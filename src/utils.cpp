// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# include "utils.hpp"

Eigen::VectorXd phgasnets::verticallyBlockVectors(
    const std::vector<Eigen::VectorXd>& vectors
){
    int total_rows = 0;
    for (const auto& vector : vectors) {
        total_rows += vector.rows();
    }

    Eigen::VectorXd result(total_rows, 1);
    int current_row = 0;

    for (const auto& vector : vectors) {
        result.block(current_row, 0, vector.rows(), 1) = vector;
        current_row += vector.rows();
    }

    return result;
}
