// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# include "utils.hpp"

Eigen::VectorXd PHModel::verticallyBlockVectors(
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

/**
 * Creates a diagonal block sparse matrix from a vector of BaseOperator references.
 *
 * @param operators a vector of references to BaseOperator objects
 *
 * @return a sparse matrix with diagonal blocks formed from the BaseOperator objects
 *
 * @throws None
 */
Eigen::SparseMatrix<double> PHModel::diagonalBlock(
    const std::vector<std::reference_wrapper<BaseOperator>>& operators
){
    int nnz = 0, n_rows = 0, n_cols = 0;
    for (const auto& operator_ : operators) {
        nnz += operator_.get().data.size(); // num of non-zeros
        n_rows += operator_.get().mat.rows(); // num rows
        n_cols += operator_.get().mat.cols(); // num cols
    }

    // Add the first operator
    std::vector<Eigen::Triplet<double>> data(nnz);
    data.insert(data.end(), operators[0].get().data.begin(), operators[0].get().data.end());
    // Iteratively include data from rest of the operators taking care of row/column offset
    int startRow = operators[0].get().mat.rows();
    int startCol = operators[0].get().mat.cols();
    for (int i = 1; i < operators.size(); ++i) {
        for(auto& triplet : operators[i].get().data) {
            data.push_back(Eigen::Triplet<double>(startRow + triplet.row(), startCol + triplet.col(), triplet.value()));
        }
        startRow += operators[i].get().mat.rows();
        startCol += operators[i].get().mat.cols();
    }

    Eigen::SparseMatrix<double> mat(n_rows, n_cols);
    mat.setFromTriplets(data.begin(), data.end());
    return mat;
}
