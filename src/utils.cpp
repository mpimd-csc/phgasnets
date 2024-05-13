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

SparseMatrix PHModel::diagonalBlock(
    const std::vector<PHModel::BasePHOperator>& operators
) {
    int nnz = 0, n_rows = 0, n_cols = 0;
    for (const auto& operator_ : operators) {
        nnz += operator_.data.size(); // Num of non-zeros
        n_rows += operator_.mat.rows(); // Num rows
        n_cols += operator_.mat.cols(); // Num cols
    }

    // Add the first operator
    std::vector<Triplet> data(nnz);
    data.insert(data.end(), operators[0].data.begin(), operators[0].data.end());
    // Iteratively include data from rest of the operators taking care of row/column offset
    int startRow = operators[0].mat.rows();
    int startCol = operators[0].mat.cols();
    for (int i = 1; i < operators.size(); ++i) {
        for(auto& triplet : operators[i].data) {
            data.push_back(Triplet(startRow + triplet.row(), startCol + triplet.col(), triplet.value()));
        }
        startRow += operators[i].mat.rows();
        startCol += operators[i].mat.cols();
    }

    SparseMatrix mat(n_rows, n_cols);
    mat.setFromTriplets(data.begin(), data.end());
    return mat;
}


/**
 * Creates a diagonal block sparse matrix from a vector of BasePHOperator references.
 *
 * @param operators a vector of references to BasePHOperator objects
 *
 * @return a sparse matrix with diagonal blocks formed from the BasePHOperator objects
 *
 * @throws None
 */
SparseMatrix PHModel::diagonalBlock2(
    const std::vector<std::reference_wrapper<BasePHOperator>>& operators
){
    int nnz = 0, n_rows = 0, n_cols = 0;
    for (const auto& operator_ : operators) {
        nnz += operator_.get().data.size(); // num of non-zeros
        n_rows += operator_.get().mat.rows(); // num rows
        n_cols += operator_.get().mat.cols(); // num cols
    }

    // Add the first operator
    std::vector<Triplet> data(nnz);
    data.insert(data.end(), operators[0].get().data.begin(), operators[0].get().data.end());
    // Iteratively include data from rest of the operators taking care of row/column offset
    int startRow = operators[0].get().mat.rows();
    int startCol = operators[0].get().mat.cols();
    for (int i = 1; i < operators.size(); ++i) {
        for(auto& triplet : operators[i].get().data) {
            data.push_back(Triplet(startRow + triplet.row(), startCol + triplet.col(), triplet.value()));
        }
        startRow += operators[i].get().mat.rows();
        startCol += operators[i].get().mat.cols();
    }

    SparseMatrix mat(n_rows, n_cols);
    mat.setFromTriplets(data.begin(), data.end());
    return mat;
}

Eigen::VectorXd PHModel::verticallyBlockVectors2(
    const std::vector<Eigen::Ref<Vector>> vectors
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
