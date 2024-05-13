// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# include <operators.hpp>
# include <vector>
# include <Eigen/Core>
# include <Eigen/SparseCore>

namespace PHModel{

Eigen::VectorXd verticallyBlockVectors(
    const std::vector<Eigen::VectorXd>& vectors
);

SparseMatrix diagonalBlock(
    const std::vector<BasePHOperator>& operators
);

SparseMatrix diagonalBlock2(
    const std::vector<std::reference_wrapper<BasePHOperator>>& operators
);

Eigen::VectorXd verticallyBlockVectors2(
    const std::vector<Eigen::Ref<Vector>> vectors
);

}