// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# include <Eigen/SparseCore>
# include "operators.hpp"
# include "network.hpp"
# include "utils.hpp"

using SparseMatrix=Eigen::SparseMatrix<double>;
using Vector=Eigen::VectorXd;
using Vector2d=Eigen::Vector2d;


namespace PHModel {
    struct TransientSystem{
        TransientSystem(
            const int n_rho, const int n_mom,
            const Vector& current_state,
            const Et_operator& Et,
            const Jt_operator& Jt,
            Rt_operator& Rt,
            Effort& effort,
            const G_operator& G,
            const Vector2d& input_vec,
            const double time,
            const double timestep
        );

        bool operator()(double const* const* guess_state, double* residual) const;

        private:
            const int n_rho, n_mom;
            const Vector& current_state;
            const Et_operator& Et;
            const Jt_operator& Jt;
            Rt_operator& Rt;
            Effort& effort;
            const G_operator& G;
            const Vector2d& input_vec;
            const double time, timestep;
    };

    struct TransientCompressorSystem{
        TransientCompressorSystem(
            Network& network,
            const Vector& current_state,
            const Eigen::Vector4d& input_vec,
            const double time,
            const double timestep
        );

        bool operator()(double const* const* guess_state, double* residual) const;

        private:
            Network& network;
            const Vector& current_state;
            const Eigen::Vector4d& input_vec;
            const double time, timestep;
    };
}