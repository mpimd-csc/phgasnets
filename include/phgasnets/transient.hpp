// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# include <Eigen/SparseCore>
# include "operators.hpp"
# include "network.hpp"
# include "utils.hpp"

namespace PHModel {
    struct TransientSystem{
        TransientSystem(
            const int n_rho, const int n_mom,
            const Eigen::Ref<const Eigen::VectorXd>& current_state,
            const Et_operator& Et,
            const Jt_operator& Jt,
            Rt_operator& Rt,
            Effort& effort,
            const G_operator& G,
            const Eigen::Ref<const Eigen::Vector2d>& input_vec,
            const double time,
            const double timestep
        );

        bool operator()(double const* const* guess_state, double* residual) const;

        private:
            const int n_rho, n_mom;
            const Eigen::VectorXd current_state;
            const Et_operator& Et;
            const Jt_operator& Jt;
            Rt_operator& Rt;
            Effort& effort;
            const G_operator& G;
            const Eigen::Vector2d input_vec;
            const double time, timestep;
    };

    struct TransientCompressorSystem{
        TransientCompressorSystem(
            Network& network,
            const Eigen::Ref<const Eigen::VectorXd>& current_state,
            const Eigen::Ref<const Eigen::Vector4d>& input_vec,
            const double time,
            const double timestep
        );

        bool operator()(double const* const* guess_state, double* residual) const;

        private:
            Network& network;
            const Eigen::VectorXd current_state;
            const Eigen::Vector4d input_vec;
            const double time, timestep;
    };
}
