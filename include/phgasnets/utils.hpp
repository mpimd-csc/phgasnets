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