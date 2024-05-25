// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later
# pragma once

# include <operators.hpp>
# include <vector>
# include <Eigen/Core>
# include <Eigen/SparseCore>

namespace PHModel{

/**
 * Creates a vertical block vector from a vector of Eigen::VectorXd objects.
 *
 * @param vectors a vector of Eigen::VectorXd objects to be vertically stacked
 *
 * @return an Eigen::VectorXd object containing the vertically stacked vectors
 *
 * @throws None
 */
Eigen::VectorXd verticallyBlockVectors(
    const std::vector<Eigen::VectorXd>& vectors
);

/**
 * Creates a diagonal block sparse matrix from a vector of BasePHOperator references.
 *
 * @param operators a vector of references to BasePHOperator objects
 *
 * @return a sparse matrix with diagonal blocks formed from the BasePHOperator objects
 *
 * @throws None
 */
SparseMatrix diagonalBlock(
    const std::vector<std::reference_wrapper<BasePHOperator>>& operators
);

}