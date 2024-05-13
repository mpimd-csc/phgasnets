#include "derivative.hpp"

using Triplet = Eigen::Triplet<double>;
using SparseMatrix = Eigen::SparseMatrix<double>;
using Matrix = Eigen::MatrixXd;
using Vector = Eigen::VectorXd;

std::vector<Triplet> derivative_operator(int N, double mesh_width) {
    std::vector<Triplet> triplets;
    triplets.reserve(3*N);

    // Central difference in interior nodes
    Vector interior_weights = taylor_table({-1, 0, 1}, 1) / mesh_width;
    for (int i = 0; i < N; ++i) {
        if (i > 0 && i < N-1) {
            // Central difference in interior nodes
            triplets.push_back(Triplet(i, i-1, interior_weights(0)));
            triplets.push_back(Triplet(i, i, interior_weights(1)));
            triplets.push_back(Triplet(i, i+1, interior_weights(2)));
        } else if (i == 0) {
            // Forward difference at head node
            Vector lbound_weights = taylor_table({0, 1, 2}, 1) / mesh_width;
            triplets.push_back(Triplet(i, i, lbound_weights(0)));
            triplets.push_back(Triplet(i, i + 1, lbound_weights(1)));
            triplets.push_back(Triplet(i, i + 2, lbound_weights(2)));
        } else if (i == N - 1) {
            // Backward difference at tail node
            Vector rbound_weights = taylor_table({-2, -1, 0}, 1) / mesh_width;
            triplets.push_back(Triplet(i, i - 2, rbound_weights(0)));
            triplets.push_back(Triplet(i, i - 1, rbound_weights(1)));
            triplets.push_back(Triplet(i, i, rbound_weights(2)));
        }
    }

    return triplets; // Return Triplets since it can be used in constructing J_mat
}

Vector taylor_table(const std::vector<double>& points, int derv_order) {
    int n = points.size();
    Matrix A(n, n);
    Vector b = Vector::Zero(n);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            A(i, j) = std::pow(points[j], i) / std::tgamma(i + 1);
        }
    }
    b(derv_order) = 1;

    return A.colPivHouseholderQr().solve(b);
}
