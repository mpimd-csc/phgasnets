// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# include <iostream>
# include <fstream>
# include <Eigen/Dense>
# include <Eigen/Sparse>
# include <chrono>
# include <ceres/ceres.h>
# include <nlohmann/json.hpp>
# include <glog/logging.h>

# include "gasconstant.hpp"
# include "utils.hpp"
# include "pipe.hpp"
# include "compressor.hpp"
# include "network.hpp"
# include "steady.hpp"
# include "transient.hpp"
# include "io.hpp"

// Define the json library
using json = nlohmann::json;

// Shorthand Types
typedef Eigen::Vector2d Vector2d;
typedef Eigen::VectorXd Vector;
typedef Eigen::SparseMatrix<double> SparseMatrix;

typedef ceres::Solver Solver;
typedef ceres::Problem Problem;
typedef ceres::CostFunction CostFunction;

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

int main(int argc, char** argv){

  google::InitGoogleLogging(argv[0]);

  using std::chrono::high_resolution_clock;
  using std::chrono::duration_cast;
  // using std::chrono::duration;
  using std::chrono::seconds;

  // Check for JSON input
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <config-file>.json\n";
    return 1; // Return an error code if not provided
  }

  // Read the JSON file
  std::ifstream config_file(argv[1]);
  json config = json::parse(config_file);

  const double R                 = config["GAS_CONSTANT"].get<double>();
  const double pipe_length       = config["pipe"]["length"].get<double>();
  const double pipe_diameter     = config["pipe"]["diameter"].get<double>();
  const double pipe_friction     = config["pipe"]["friction"].get<double>();
  const double inlet_temperature = config["boundary_conditions"]["inlet"]["temperature"].get<double>();
  const double inlet_pressure    = config["boundary_conditions"]["inlet"]["pressure"].get<double>();
  const double compr_ratio       = config["compressor"]["compression_ratio"].get<double>();
  const double kappa             = config["fluid"]["isentropic_exponent"].get<double>();
  const int    Nx                = config["discretization"]["space"]["resolution"].get<int>();

  // Set model-wide constants
  PHModel::set_gas_constant(R);

  // // Make Pipes and Compressor objects
  std::vector<PHModel::Compressor> compressors = {
        PHModel::Compressor("FC", "AV", compr_ratio, kappa)
  };

  const double outlet_temperature = inlet_temperature * compressors[0].temperature_scale;

  std::vector<PHModel::DiscretePipe> pipes = {
    PHModel::DiscretePipe(pipe_length, pipe_diameter, pipe_friction, inlet_temperature, Nx),
    PHModel::DiscretePipe(pipe_length, pipe_diameter, pipe_friction, outlet_temperature, Nx)
  };

  PHModel::Network network(pipes, compressors);

  // ------------------------------------------------------------------------
  auto t1 = high_resolution_clock::now();

  // Steady State Solve
  const double p0   = config["initial_conditions"]["pressure"].get<double>();
  const double mom0 = config["initial_conditions"]["momentum"].get<double>();

  Vector pipeL_init_density(Nx+1), pipeL_init_momentum(Nx+1);
  pipeL_init_density.setConstant(p0/(R*inlet_temperature));
  pipeL_init_momentum.setConstant(mom0/compressors[0].momentum_scale);

  Vector pipeR_init_density(Nx+1), pipeR_init_momentum(Nx+1);
  pipeR_init_density.setConstant(p0*compr_ratio/(R*outlet_temperature));
  pipeR_init_momentum.setConstant(mom0);

  Vector init_state = PHModel::verticallyBlockVectors({
    pipeL_init_density, pipeL_init_momentum,
    pipeR_init_density, pipeR_init_momentum
  });

  Eigen::Vector4d u_b({
    inlet_pressure,
    1.0/compressors[0].momentum_scale,
    compressors[0].compression_ratio,
    -momentum_at_outlet(0.0)
  });

  Problem problem_steady;
  auto cost_function_steady = new ceres::DynamicNumericDiffCostFunction<PHModel::SteadyCompressorSystem>(
      new PHModel::SteadyCompressorSystem(network, u_b)
  );

  cost_function_steady->AddParameterBlock(network.n_state);
  cost_function_steady->SetNumResiduals(network.n_res);

  problem_steady.AddResidualBlock(cost_function_steady, nullptr, init_state.data());

  // Set CERES Solver Options
  Solver::Summary summary;
  Solver::Options options;
  options.function_tolerance = 1e-8;
  options.max_num_iterations = 2000;
  options.num_threads        = 8;

  ceres::Solve(options, &problem_steady, &summary);
  auto t2       = high_resolution_clock::now();
  auto duration = duration_cast<seconds>( t2 - t1 );
  std::cout << "Steady solution computed in " << duration.count() << "s\n";
  network.set_gas_state(init_state);

  // ------------------------------------------------------------------------
  // Transient Solve
  const double t_start = config["discretization"]["time"]["start"].get<double>();
  const double t_end   = config["discretization"]["time"]["end"].get<double>();
  const double dt      = config["discretization"]["time"]["step"].get<double>();
  const int    Nt      = std::ceil((t_end - t_start)*3600/dt);
  float        time;

  Vector current_state = init_state;
  Vector guess         = init_state;

  // Read config for io frequency and filename
  int io_frequency     = config["io"]["frequency"].get<int>();
  std::string filename = config["io"]["filename"].get<std::string>();

  PHModel::NetworkStateWriter network_writer(filename+".h5", network);
  network_writer.writeMesh();
  network_writer.writeState(0, 0);

  t1 = high_resolution_clock::now();

  // Time Loop
  for (int t=1; t<Nt; ++t) {

    time = t_start*3600 + t * dt;
    std::cout << "Time = " << time << "s (" << t << "/" << Nt << ")\r";

    // Update guess at inlet
    guess(0)                 = inlet_pressure/ (R*inlet_temperature);
    guess(network.n_state-1) = momentum_at_outlet(time);

    // Update input vector
    u_b(3) = -(momentum_at_outlet(time) + momentum_at_outlet(time-dt)) * 0.5;

    Problem problem_transient;
    auto cost_function_transient =
      new ceres::DynamicNumericDiffCostFunction<PHModel::TransientCompressorSystem>(
        new PHModel::TransientCompressorSystem(network, current_state, u_b, time, dt)
    );

    cost_function_transient->AddParameterBlock(network.n_state);
    cost_function_transient->SetNumResiduals(network.n_res);
    problem_transient.AddResidualBlock(cost_function_transient, nullptr, guess.data());

    Solver::Options options;
    Solver::Summary summary;
    options.function_tolerance = 1e-8;
    options.max_num_iterations = 2000;
    options.num_threads        = 8;
    ceres::Solve(options, &problem_transient, &summary);

    current_state = guess; // Set the current state to the new solution
    network.set_gas_state(current_state);

    // IO
    if (t % io_frequency == 0)
        network_writer.writeState(t, time);
  }

  std::cout << "Results written in [" << filename << "]" << std::endl;

  t2       = high_resolution_clock::now();
  duration = duration_cast<seconds>( t2 - t1 );
  std::cout << "Transient solution computed in " << duration.count() << "s\t ("
            << duration.count()/(float)Nt << "s per timestep).\n";

  return 0;

}