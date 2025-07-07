// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# include <iostream>
# include <fstream>
# include <array>
# include <argparse/argparse.hpp>
# include <Eigen/Dense>
# include <Eigen/Sparse>
# include <chrono>
# include <ceres/ceres.h>
# include <nlohmann/json.hpp>
# include <phgasnets>

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

int main(int argc, char** argv){

  using std::chrono::high_resolution_clock;
  using std::chrono::duration_cast;
  using std::chrono::seconds;

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

  const double inlet_temperature = config["boundary_conditions"]["inlet"]["temperature"].get<double>();
  const double inlet_pressure    = config["boundary_conditions"]["inlet"]["pressure"].get<double>();
  const double compr_spec        = config["compressor"]["specification"].get<double>();
  const double kappa             = config["fluid"]["isentropic_exponent"].get<double>();
  const int    Nx                = config["discretization"]["space"]["resolution"].get<int>();

  // Set model-wide constants
  phgasnets::set_gas_constant(config["GAS_CONSTANT"]);

  // // Make Pipes and Compressor objects
  std::vector<phgasnets::Compressor> compressors = {
        phgasnets::Compressor(config["compressor"], kappa)
  };

  const double outlet_temperature = inlet_temperature * compressors[0].temperature_scale;

  std::vector<phgasnets::Pipe> pipes = {
    phgasnets::Pipe(config["pipe"], inlet_temperature),
    phgasnets::Pipe(config["pipe"], outlet_temperature)
  };

  phgasnets::Network net = phgasnets::Network(pipes, compressors);

  auto network = phgasnets::discretize<double>(net, config["discretization"]["space"]);

  // ------------------------------------------------------------------------
  auto t1 = high_resolution_clock::now();

  // Steady State Solve
  const double p0   = config["initial_conditions"]["pressure"].get<double>();
  const double mom0 = config["initial_conditions"]["momentum"].get<double>();

  // initial guess
  Vector pipeL_init_density(Nx+1), pipeL_init_momentum(Nx+1);
  pipeL_init_density.setConstant(p0/(phgasnets::GAS_CONSTANT*inlet_temperature));
  pipeL_init_momentum.setConstant(mom0/network.compressors[0].momentum_scale);

  if (network.compressors[0].type == "FP") {
    network.compressors[0].update_compression_ratio(compr_spec/p0);
  }
  else if (network.compressors[0].type == "FC") {
    network.compressors[0].update_compression_ratio(compr_spec);
  }

  Vector pipeR_init_density(Nx+1), pipeR_init_momentum(Nx+1);
  pipeR_init_density.setConstant(p0*network.compressors[0].compression_ratio/(phgasnets::GAS_CONSTANT*outlet_temperature));
  pipeR_init_momentum.setConstant(mom0);

  Vector init_state = phgasnets::verticallyBlockVectors({
    pipeL_init_density, pipeL_init_momentum,
    pipeR_init_density, pipeR_init_momentum
  });

  // boundary conditions
  Eigen::Vector4d u_b({
    inlet_pressure,
    0.0,
    network.compressors[0].specification,
    -momentum_at_outlet(0.0)
  });

  if (network.compressors[0].model == "AV") {
    u_b(1) = 1.0/std::pow(network.compressors[0].specification, 1/network.compressors[0].isentropic_exponent);
  }
  else if (network.compressors[0].model == "AM") {
    u_b(1) = 1.0;
  }

  // configure solver
  Problem problem_steady;
  auto cost_function_steady = new DynamicDiffCostFunction<phgasnets::SteadyCompressorSystem>(
      new phgasnets::SteadyCompressorSystem(net, config["discretization"]["space"], u_b)
  );

  cost_function_steady->AddParameterBlock(network.n_state);
  cost_function_steady->SetNumResiduals(network.n_res);

  problem_steady.AddResidualBlock(cost_function_steady, nullptr, init_state.data());

  // Set CERES Solver Options
  Solver::Summary summary;
  Solver::Options options;
  options.function_tolerance = 1e-12;
  options.max_num_iterations = 2000;
  options.num_threads        = 1;

  ceres::Solve(options, &problem_steady, &summary);
  auto t2       = high_resolution_clock::now();
  auto duration = duration_cast<seconds>( t2 - t1 );
  std::cout << "Steady solution computed in " << duration.count() << "s\n";

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

  phgasnets::NetworkStateWriter network_writer(filename+".h5", network);
  network_writer.writeMesh();
  network_writer.writeState(0, 0);

  // CSV write out: Initialize vectors
  std::vector<double> timestamps(Nt);
  std::vector<std::vector<double>> inflow_pressure;
  std::vector<std::vector<double>> outflow_pressure;
  std::vector<std::vector<double>> inflow_momentum;
  std::vector<std::vector<double>> outflow_momentum;

  std::size_t i = 0;
  for(auto& pipe: network.pipes){
    double RT  = phgasnets::GAS_CONSTANT*pipe.temperature;

    timestamps[0] = 0.0;
    inflow_pressure.push_back(std::vector<double>(Nt));
    outflow_pressure.push_back(std::vector<double>(Nt));
    inflow_momentum.push_back(std::vector<double>(Nt));
    outflow_momentum.push_back(std::vector<double>(Nt));

    inflow_pressure[i][0] = pipe.rho(0)*RT/1e5;
    outflow_pressure[i][0] = pipe.rho(Eigen::last)*RT/1e5;
    inflow_momentum[i][0] = pipe.mom(0);
    outflow_momentum[i][0] = pipe.mom(Eigen::last);

    ++i;
  }

  t1 = high_resolution_clock::now();

  Problem problem_transient;
  auto cost_function_transient =
    new DynamicDiffCostFunction<phgasnets::TransientCompressorSystem>(
      new phgasnets::TransientCompressorSystem(net, config["discretization"], current_state, u_b, time)
  );

  cost_function_transient->AddParameterBlock(network.n_state);
  cost_function_transient->SetNumResiduals(network.n_res);
  problem_transient.AddResidualBlock(cost_function_transient, nullptr, guess.data());

  // Time Loop
  for (int t=1; t<Nt; ++t) {

    time = t_start*3600 + t * dt;
    std::cout << "Time = " << time << "s (" << t << "/" << Nt << ")\r";

    // Update guess at inlet
    guess(0)                 = inlet_pressure/(phgasnets::GAS_CONSTANT*inlet_temperature);
    guess(network.n_state-1) = momentum_at_outlet(time);

    // Update input vector
    u_b(3) = -(momentum_at_outlet(time) + momentum_at_outlet(time-dt)) * 0.5;

    ceres::Solve(options, &problem_transient, &summary);

    current_state = guess; // Set the current state to the new solution
    network.set_state(current_state);

    // IO
    if (t % io_frequency == 0) {
        network_writer.writeState(t, time);

        std::size_t i = 0;
        for(auto& pipe: network.pipes){
          double RT  = phgasnets::GAS_CONSTANT*pipe.temperature;

          timestamps[t] = time/3600.0;

          inflow_pressure[i][t] = pipe.rho(0)*RT/1e5;
          outflow_pressure[i][t] = pipe.rho(Eigen::last)*RT/1e5;
          inflow_momentum[i][t] = pipe.mom(0);
          outflow_momentum[i][t] = pipe.mom(Eigen::last);

          ++i;
        }
    }
  }

  std::cout << "Results written in [" << filename << "]" << std::endl;

  if (parser.get<bool>("csv")) {

    std::size_t i = 0;
    for(auto& pipe: network.pipes){
      phgasnets::writeColumnsToCSV(
        filename+"_pipe"+std::to_string(i)+".csv",
        {
          "time",
          "inletPressure",
          "outletPressure",
          "inletMomentum",
          "outletMomentum"
        },
        {
          timestamps,
          inflow_pressure[i],
          outflow_pressure[i],
          inflow_momentum[i],
          outflow_momentum[i]
        }
      );

      std::cout
        << "CSV file written in ["
        << filename+"_pipe"+std::to_string(i)+".csv"
        << "]"
        << std::endl;

      ++i;
    }

  }

  t2       = high_resolution_clock::now();
  duration = duration_cast<seconds>( t2 - t1 );
  std::cout << "Transient solution computed in " << duration.count() << "s\t ("
            << duration.count()/(float)Nt << "s per timestep).\n";

  return 0;

}
