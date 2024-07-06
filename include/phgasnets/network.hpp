// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# pragma once
# include "pipe.hpp"
# include "compressor.hpp"
# include "utils.hpp"
# include <vector>

namespace phgasnets {
  struct DiscreteNetwork {

    DiscreteNetwork(std::vector<DiscretePipe<double>>& pipes, std::vector<Compressor>& compressors);

    void set_gas_state(const Eigen::Ref<const Eigen::VectorXd>& state);

    public:
      std::vector<DiscretePipe<double>>& pipes;
      std::vector<Compressor>& compressors;
      Eigen::SparseMatrix<double> E, J, R, G;
      Eigen::VectorXd effort;
      int n_state, n_res;
  };

}
