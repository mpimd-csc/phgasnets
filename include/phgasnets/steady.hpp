# include <Eigen/SparseCore>
# include "operators.hpp"
# include "pipe.hpp"
# include "compressor.hpp"
# include "network.hpp"
# include "utils.hpp"

using SparseMatrix=Eigen::SparseMatrix<double>;
using Vector=Eigen::VectorXd;
using Vector2d=Eigen::Vector2d;


namespace PHModel {
    struct SteadySystem{
        SteadySystem(
            const int n_rho, const int n_mom,
            const Jt_operator& Jt,
            Rt_operator& Rt,
            Effort& effort,
            const G_operator& G,
            const Vector2d& input_vec
        );

        bool operator()(double const* const* guess_state, double* residual) const;

        private:
            const int n_rho, n_mom;
            const Jt_operator& Jt;
            Rt_operator& Rt;
            Effort& effort;
            const G_operator& G;
            const Vector2d& input_vec;
    };

    struct SteadyCompressorSystem{
        SteadyCompressorSystem(
            Network& network,
            const Eigen::Vector4d& input_vec
        );
        bool operator()(double const* const* guess_state, double* residual) const;
        private:
            Network& network;
            const Eigen::Vector4d& input_vec;
    };
}