// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# pragma once

# include "operators.hpp"

# include <vector>
# include <Eigen/Core>
# include <Eigen/SparseCore>

namespace phgasnets{

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
 * Creates a diagonal block sparse matrix from a vector of BaseOperator references.
 *
 * @param operators a vector of references to BaseOperator objects
 *
 * @return a sparse matrix with diagonal blocks formed from the BaseOperator objects
 *
 * @throws None
 */
template <typename T>
Eigen::SparseMatrix<T> diagonalBlock(
    const std::vector<std::reference_wrapper<BaseOperator<T>>>& operators
){
    int nnz = 0, n_rows = 0, n_cols = 0;
    for (const auto& operator_ : operators) {
        nnz += operator_.get().data.size(); // num of non-zeros
        n_rows += operator_.get().mat.rows(); // num rows
        n_cols += operator_.get().mat.cols(); // num cols
    }

    // Add the first operator
    std::vector<Eigen::Triplet<T>> data(nnz);
    data.insert(data.end(), operators[0].get().data.begin(), operators[0].get().data.end());
    // Iteratively include data from rest of the operators taking care of row/column offset
    int startRow = operators[0].get().mat.rows();
    int startCol = operators[0].get().mat.cols();
    for (int i = 1; i < operators.size(); ++i) {
        for(auto& triplet : operators[i].get().data) {
            data.push_back(Eigen::Triplet<T>(startRow + triplet.row(), startCol + triplet.col(), triplet.value()));
        }
        startRow += operators[i].get().mat.rows();
        startCol += operators[i].get().mat.cols();
    }

    Eigen::SparseMatrix<T> mat(n_rows, n_cols);
    mat.setFromTriplets(data.begin(), data.end());
    return mat;
}

/*
 * Writes a vector of vectors of doubles to a CSV file.
 *
 * @param filename the name of the file to write to
 * @param column_names the names of the columns
 * @param columns the data to write
 *
 * @throws None
 */
void writeColumnsToCSV(
    const std::string& filename,
    const std::vector<std::string>& column_names,
    const std::vector<std::vector<double>>& columns
);

}
