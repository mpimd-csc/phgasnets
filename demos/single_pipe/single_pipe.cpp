// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <ceres/ceres.h>
#include "operators.hpp"
#include "state_operators.hpp"
#include "steady.hpp"
#include "transient.hpp"
#include <highfive/H5Easy.hpp>

// Define the json library
using json = nlohmann::json;

// Shorthand Types
typedef Eigen::Vector2d Vector2d;
typedef Eigen::VectorXd Vector;
typedef Eigen::SparseMatrix<double> SparseMatrix;

typedef ceres::Solver Solver;
typedef ceres::Problem Problem;
typedef ceres::CostFunction CostFunction;

// Switch between numeric and auto differentiation
#if PHGASNETS_NUMERICDIFF
template<typename T>
  using DynamicDiffCostFunction = ceres::DynamicNumericDiffCostFunction<T>;
#else
template<typename T>
  using DynamicDiffCostFunction = ceres::DynamicAutoDiffCostFunction<T>;
#endif

double momentum_at_outlet(double time) {
  if (time < 6*3600) {
      return 463.33;
  } else if (time < 12*3600) {
      return 540.55;
  } else if (time < 18*3600) {
      return 386.11;
  } else {
      return 463.33;
  }
}

int main(int argc, char** argv) {
  // Check for JSON input
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <config-file>.json\n";
    return 1; // Return an error code if not provided
  }

  // Read the JSON file
  std::ifstream config_file(argv[1]);
  json config = json::parse(config_file);

  const double R              = config["GAS_CONSTANT"].get<double>();
  const double temperature    = config["fluid"]["temperature"].get<double>();
  const double RT             = R * temperature;
  const double pipe_length    = config["pipe"]["length"].get<double>();
  const double pipe_diameter  = config["pipe"]["diameter"].get<double>();
  const double pipe_friction  = config["pipe"]["friction"].get<double>();
  const double inlet_pressure = config["boundary_conditions"]["inlet"]["pressure"].get<double>();
  const int    Nx             = config["discretization"]["space"]["resolution"].get<int>();

  // Create mesh
  Vector mesh       = Vector::LinSpaced(Nx + 1, 0.0, pipe_length);
  double mesh_width = pipe_length / Nx;

  // Define the size of the system
  const int n_rho = Nx+1;
  const int n_mom = Nx+1;

  // Create Port Hamiltonian Operators
  PHModel::set_gas_constant(R);
  auto Et     = PHModel::Et_operator(n_rho, n_mom);
  auto Jt     = PHModel::Jt_operator(n_rho, n_mom, mesh_width);
  auto G      = PHModel::G_operator(n_rho, n_mom);
  auto u_b    = PHModel::input_vec(inlet_pressure, momentum_at_outlet(0.0));

  // ------------------------------------------------------------------------
  // Steady State Solve

  // initial state
  Vector init_state(n_rho+n_mom);
  const double rho0 = config["initial_conditions"]["pressure"].get<double>()/RT;
  const double mom0 = config["initial_conditions"]["momentum"].get<double>();
  init_state.segment(0, n_rho).setConstant(rho0);
  init_state.segment(n_rho, n_mom).setConstant(mom0);

  // SteadyState
  Problem problem_steady;
  auto cost_function_steady = new DynamicDiffCostFunction<PHModel::SteadySystem>(
      new PHModel::SteadySystem(
        n_rho, n_mom, Jt, G, pipe_friction, pipe_diameter, temperature, u_b
      )
  );

  cost_function_steady->AddParameterBlock(n_rho+n_mom);
  cost_function_steady->SetNumResiduals(n_rho+n_mom+2);

  // Second argument is the loss func.
  // in our case is the identity function (default), so we get a non-linear least squares problem.
  problem_steady.AddResidualBlock(cost_function_steady, nullptr, init_state.data());

  // Set CERES Solver Options
  Solver::Summary summary;
  Solver::Options options;
  options.function_tolerance = 1e-8;
  options.max_num_iterations = 2000;
  options.num_threads = 8;

  ceres::Solve(options, &problem_steady, &summary);

  // ------------------------------------------------------------------------
  // Transient Solve
  const double t_start = config["discretization"]["time"]["start"].get<double>();
  const double t_end   = config["discretization"]["time"]["end"].get<double>();
  const double dt      = config["discretization"]["time"]["step"].get<double>();
  const int    Nt      = int((t_end - t_start)*3600/dt);
  float        time;

  Vector current_state = init_state;
  Vector guess = init_state;

  // Read config for io frequency and filename
  int io_frequency = config["io"]["frequency"].get<int>();
  std::string filename = config["io"]["filename"].get<std::string>()+".h5";
  H5Easy::File file(filename, H5Easy::File::Truncate);
  {
    Vector rho = current_state(Eigen::seqN(0, n_rho));
    Vector mom = current_state(Eigen::seqN(n_rho, n_mom));
    H5Easy::dump(file, "/mesh", mesh);
    H5Easy::dump(file, "/0/density", rho);
    H5Easy::dump(file, "/0/momentum", mom);
    H5Easy::dump(file, "/0/timestamp", 0.0);
  }

  for (int t=1; t<Nt; ++t) {
    time = t_start*3600 + t * dt;

    guess(0) = inlet_pressure/RT;
    guess(n_rho+n_mom-1) = momentum_at_outlet(time);
    u_b = (
      PHModel::input_vec(inlet_pressure, momentum_at_outlet(time))
      + PHModel::input_vec(inlet_pressure, momentum_at_outlet(time-dt))
    ) * 0.5;

    Problem problem_transient;
    auto cost_function_transient =
        new DynamicDiffCostFunction<PHModel::TransientSystem>(
            new PHModel::TransientSystem(
              n_rho, n_mom, current_state, Et, Jt, G, pipe_friction, pipe_diameter, temperature, u_b, time, dt
            )
    );

    cost_function_transient->AddParameterBlock(n_rho+n_mom);
    cost_function_transient->SetNumResiduals(n_rho+n_mom+2);
    problem_transient.AddResidualBlock(cost_function_transient, nullptr, guess.data());

    Solver::Options options;
    Solver::Summary summary;
    options.function_tolerance = 1e-8;
    options.max_num_iterations = 2000;
    options.num_threads = 4;
    ceres::Solve(options, &problem_transient, &summary);

    current_state = guess; // Set the current state to the new solution

    // IO
    if (t % io_frequency == 0){
        Vector rho = current_state(Eigen::seqN(0, n_rho));
        Vector mom = current_state(Eigen::seqN(n_rho, n_mom));
        H5Easy::dump(file, "/" + std::to_string(t) + "/density", rho);
        H5Easy::dump(file, "/" + std::to_string(t) + "/momentum", mom);
        H5Easy::dump(file, "/" + std::to_string(t) + "/timestamp", time);
    }
  }

  std::cout << "Results written in [" << filename << "]" << std::endl;
  // ------------------------------------------------------------------------
  // Orderly exit
  return 0;
}
