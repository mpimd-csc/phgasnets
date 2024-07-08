// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# pragma once

# include <nlohmann/json.hpp>
# include <ceres/jet.h>
# include <vector>

# include "operators.hpp"
# include "state_operators.hpp"
# include "pipe.hpp"
# include "compressor.hpp"
# include "utils.hpp"

namespace phgasnets {

  struct Network{
    Network(
      std::vector<Pipe>& pipes,
      std::vector<Compressor>& compressors
    ): pipes(pipes), compressors(compressors)
    {}

    public:
      std::vector<Pipe>& pipes;
      std::vector<Compressor>& compressors;
  };

  template<typename T>
  struct DiscreteNetwork {

    DiscreteNetwork(
      std::vector<DiscretePipe<T>>& pipes,
      std::vector<Compressor>& compressors
    ):
      pipes(pipes), compressors(compressors), n_state(0), n_res(0)
    {
      // diagonally block static operators
      std::vector<std::reference_wrapper<BaseOperator>> operators_e, operators_j;
      std::vector<std::reference_wrapper<BaseStateOperator<T>>> operators_r, operators_g;
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
      R = diagonalBlockT<T>(operators_r);
      G = diagonalBlockT<T>(operators_g);
      effort.resize(n_res);
    };

    void set_state(const Eigen::Ref<const Eigen::Vector<T, Eigen::Dynamic>>& state) {
      // distribute state across network constituents
      int pipe_state_startIdx = 0;
      for (auto& pipe : pipes) {
        auto pipe_state = state(Eigen::seqN(pipe_state_startIdx, pipe.n_state));
        pipe.set_state(pipe_state);
        pipe_state_startIdx += pipe.n_state;
      }
      auto precompressor_pressure = phgasnets::GAS_CONSTANT * pipes[0].temperature * pipes[0].rho(Eigen::last);

      // set temperatures for post-compressor pipes
      if (compressors[0].type == "FP") {
        compressors[0].update_compression_ratio(compressors[0].specification/precompressor_pressure);
      }

      // update pipe-specific R and effort
      std::vector<std::reference_wrapper<BaseStateOperator<T>>> operators_r, operators_g;

      int pipe_res_startIdx = 0;
      for(auto& pipe : pipes){
        operators_r.push_back(std::ref(pipe.Rt));
        effort(Eigen::seqN(pipe_res_startIdx, pipe.n_res)) = pipe.effort.vec_t;
        pipe_res_startIdx += pipe.n_res;
      }
      auto postcompressor_momentum = pipes[1].mom(0);

      // build network R operator
      R = diagonalBlockT<T>(operators_r);

      // update network G operator
      G.coeffRef(pipes[0].n_res-1, 1) = T(-postcompressor_momentum);
      if (compressors[0].type == "FC") {
        G.coeffRef(pipes[0].n_res+pipes[1].n_res-2, 2) = T(precompressor_pressure);
      }
      else {
        if (compressors[0].type == "FP" && compressors[0].model == "AV") {
          G.coeffRef(pipes[0].n_res-1, 1) *= T(ceres::pow(precompressor_pressure, 1.0/compressors[0].isentropic_exponent));
        }
      }
    }

    public:
      std::vector<DiscretePipe<T>> pipes;
      std::vector<Compressor> compressors;
      Eigen::SparseMatrix<double> E, J;
      Eigen::SparseMatrix<T> R, G;
      Eigen::Vector<T, Eigen::Dynamic> effort;
      int n_state, n_res;
  };

  template<typename T>
  DiscreteNetwork<T> discretize(const Network& network, const nlohmann::json& spatial_disc_params){

    std::vector<DiscretePipe<T>> discrete_pipes;
    int Nx = spatial_disc_params["resolution"];

    for (auto& pipe: network.pipes)
      discrete_pipes.push_back(DiscretePipe<T>(pipe, Nx));

    return DiscreteNetwork<T>(discrete_pipes, network.compressors);
  }
} // namespace phgasnets
