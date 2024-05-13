# include <vector>
# include <Eigen/Core>
# include <Eigen/QR>
# include <Eigen/SparseCore>


std::vector<Eigen::Triplet<double>> derivative_operator(int N, double mesh_width);

Eigen::VectorXd taylor_table(const std::vector<double>& points, int derv_order);