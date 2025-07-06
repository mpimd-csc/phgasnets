// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

#include <iostream>
#include <fstream>
#include <argparse/argparse.hpp>
#include <nlohmann/json.hpp>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <ceres/ceres.h>
#include <highfive/H5Easy.hpp>
#include "phgasnets.hpp"

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
#pragma message("Using NumericDiff")
template<typename T>
  using DynamicDiffCostFunction = ceres::DynamicNumericDiffCostFunction<T>;
#else
#pragma message("Using AutoDiff")
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
  argparse::ArgumentParser parser("single_pipe", "1.0", argparse::default_arguments::help);
  parser.add_argument("-c", "--config")
    .help("Path to the <config-file>.json")
    .default_value("config.json")
    .required();

  parser.add_argument("--csv")
    .help("Flag to output <csv-file>.csv")
    .default_value(false)
    .implicit_value(true);

  try {
    parser.parse_args(argc, argv);
  }
  catch (const std::exception& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << parser;
    std::exit(1);
  }
  // Read the JSON file
  std::ifstream config_file(parser.get<std::string>("config"));
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
  phgasnets::set_gas_constant(R);
  auto Et     = phgasnets::Et_operator(n_rho, n_mom);
  auto Jt     = phgasnets::Jt_operator(n_rho, n_mom, mesh_width);
  auto G      = phgasnets::G_operator<double>(n_rho, n_mom);
  Eigen::Vector2d u_b({inlet_pressure, -momentum_at_outlet(0.0)});

  // ------------------------------------------------------------------------
  // Set CERES Solver Options
  Solver::Summary summary;
  Solver::Options options;
  options.function_tolerance = 1e-8;
  options.max_num_iterations = 2000;
  options.num_threads = 1; // runtime threads
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
  auto cost_function_steady = new DynamicDiffCostFunction<phgasnets::SteadySystem>(
      new phgasnets::SteadySystem(
        n_rho, n_mom, Jt, G, pipe_friction, pipe_diameter, temperature, u_b
      )
  );

  cost_function_steady->AddParameterBlock(n_rho+n_mom);
  cost_function_steady->SetNumResiduals(n_rho+n_mom+2);

  // Second argument is the loss func.
  // in our case is the identity function (default), so we get a non-linear least squares problem.
  problem_steady.AddResidualBlock(cost_function_steady, nullptr, init_state.data());

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
  std::string filename = config["io"]["filename"].get<std::string>();
  H5Easy::File file(filename+".h5", H5Easy::File::Truncate);

  // CSV write out: Init
  std::vector<double>
    timestamps(Nt),
    inflow_pressure(Nt),
    outflow_pressure(Nt),
    inflow_momentum(Nt),
    outflow_momentum(Nt);

  {
    Vector rho = current_state(Eigen::seqN(0, n_rho));
    Vector mom = current_state(Eigen::seqN(n_rho, n_mom));

    H5Easy::dump(file, "/mesh", mesh);
    H5Easy::dump(file, "/0/density", rho);
    H5Easy::dump(file, "/0/momentum", mom);
    H5Easy::dump(file, "/0/timestamp", 0.0);

    timestamps[0] = 0.0;
    inflow_pressure[0] = rho(0)*RT/1e5;
    outflow_pressure[0] = rho(Eigen::last)*RT/1e5;
    inflow_momentum[0] = mom(0);
    outflow_momentum[0] = mom(Eigen::last);
  }

  Problem problem_transient;
  auto cost_function_transient =
      new DynamicDiffCostFunction<phgasnets::TransientSystem>(
          new phgasnets::TransientSystem(
            n_rho, n_mom, current_state, Et, Jt, G, pipe_friction, pipe_diameter, temperature, u_b, time, dt
          )
  );

  cost_function_transient->AddParameterBlock(n_rho+n_mom);
  cost_function_transient->SetNumResiduals(n_rho+n_mom+2);
  problem_transient.AddResidualBlock(cost_function_transient, nullptr, guess.data());

  options.function_tolerance = 1e-8;

  // Time Loop
  for (int t=1; t<Nt; ++t) {
    time = t_start*3600 + t * dt;

    guess(0) = inlet_pressure/RT;
    guess(n_rho+n_mom-1) = momentum_at_outlet(time);
    u_b = (
      Eigen::Vector2d({inlet_pressure, -momentum_at_outlet(time)})
      + Eigen::Vector2d({inlet_pressure, -momentum_at_outlet(time-dt)})
    ) * 0.5;

    ceres::Solve(options, &problem_transient, &summary);

    current_state = guess; // Set the current state to the new solution

    // IO
    if (t % io_frequency == 0){
        Vector rho = current_state(Eigen::seqN(0, n_rho));
        Vector mom = current_state(Eigen::seqN(n_rho, n_mom));
        H5Easy::dump(file, "/" + std::to_string(t) + "/density", rho);
        H5Easy::dump(file, "/" + std::to_string(t) + "/momentum", mom);
        H5Easy::dump(file, "/" + std::to_string(t) + "/timestamp", time);

        timestamps[t] = time/3600.0;
        inflow_pressure[t] = rho(0)*RT/1e5;
        outflow_pressure[t] = rho(Eigen::last)*RT/1e5;
        inflow_momentum[t] = mom(0);
        outflow_momentum[t] = mom(Eigen::last);
    }
  }

  std::cout
    << "Results written in ["
    << filename+".h5"
    << "]"
    << std::endl;

  if (parser.get<bool>("csv")) {
    phgasnets::writeColumnsToCSV(
      filename+".csv",
      {
        "time",
        "inletPressure",
        "outletPressure",
        "inletMomentum",
        "outletMomentum"
      },
      {
        timestamps,
        inflow_pressure,
        outflow_pressure,
        inflow_momentum,
        outflow_momentum
      }
    );

    std::cout
      << "CSV file written in ["
      << filename+".csv"
      << "]"
      << std::endl;
  }

  // ------------------------------------------------------------------------
  // Orderly exit
  return 0;
}
