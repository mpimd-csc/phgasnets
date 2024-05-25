// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# include "network.hpp"

namespace PHModel {
Network::Network(
  std::vector<DiscretePipe>& pipes,
  std::vector<Compressor>& compressors
) :
  pipes(pipes), compressors(compressors), n_state(0), n_res(0)
{
  // diagonally block static operators
  std::vector<std::reference_wrapper<BasePHOperator>> operators_e, operators_j, operators_r, operators_g;
  for (auto& pipe: pipes) {
    operators_e.push_back(std::ref(pipe.Et));
    operators_j.push_back(std::ref(pipe.Jt));
    operators_r.push_back(std::ref(pipe.Rt));
    operators_g.push_back(std::ref(pipe.G));
    n_state += pipe.n_state;
    n_res += pipe.n_res;
  }
  E = diagonalBlock(operators_e);
  J = diagonalBlock(operators_j);
  R = diagonalBlock(operators_r);
  G = diagonalBlock(operators_g);
  effort.resize(n_res);
}

void Network::set_gas_state(const Vector& state) {
  // distribute state across network constituents
  int pipe_state_startIdx = 0;
  for (auto& pipe : pipes) {
    auto pipe_state = state(Eigen::seqN(pipe_state_startIdx, pipe.n_state));
    pipe.set_gas_state(pipe_state);
    pipe_state_startIdx += pipe.n_state;
  }
  auto precompressor_pressure = pipes[0].get_pressure()(Eigen::last);

  // set temperatures for post-compressor pipes
  if (compressors[0].type == "FP") {
    compressors[0].update_compression_ratio(compressors[0].specification/precompressor_pressure);
  }
  // pipes[1].temperature = compressors[0].temperature_scale * pipes[0].temperature;

  // update pipe-specific R and effort
  std::vector<std::reference_wrapper<BasePHOperator>> operators_r, operators_g;
  int pipe_res_startIdx = 0;
  for(auto& pipe : pipes){
    operators_r.push_back(std::ref(pipe.Rt));
    effort(Eigen::seqN(pipe_res_startIdx, pipe.n_res)) = pipe.effort.vec_t;
    pipe_res_startIdx += pipe.n_res;
  }
  auto postcompressor_momentum = pipes[1].mom(0);

  // build network R operator
  R = diagonalBlock(operators_r);

  // update network G operator
  G.coeffRef(pipes[0].n_res-1, 1) = -postcompressor_momentum;
  if (compressors[0].type == "FC") {
    G.coeffRef(pipes[0].n_res+pipes[1].n_res-2, 2) = precompressor_pressure;
  }
  else {
    if (compressors[0].type == "FP" && compressors[0].model == "AV") {
      G.coeffRef(pipes[0].n_res-1, 1) *= std::pow(precompressor_pressure, 1.0/compressors[0].isentropic_exponent);
    }
  }
}

Vector Network::get_gas_state() const {
  std::vector<Vector> states;
  for (auto& pipe: pipes) {
    states.push_back(pipe.get_gas_state());
  }

  return verticallyBlockVectors(states);
}

} // namespace PHModel