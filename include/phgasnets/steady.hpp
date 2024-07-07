// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# include <Eigen/SparseCore>
# include "operators.hpp"
# include "state_operators.hpp"
# include "pipe.hpp"
# include "compressor.hpp"
# include "network.hpp"
# include "utils.hpp"

namespace phgasnets {
    struct SteadySystem{
        SteadySystem(
            const int n_rho, const int n_mom,
            const Jt_operator& Jt,
            const G_operator& G,
            const double& friction,
            const double& diameter,
            const double& temperature,
            const Eigen::Vector2d& input_vec
        ): n_rho(n_rho), n_mom(n_mom), Jt(Jt), G(G), input_vec(input_vec), f(friction), D(diameter), temperature(temperature)
        {}

        template <typename T>
        bool operator()(T const* const* guess_state, T* residual) const {
            // Distribute parameters into density and momentum vectors
            Eigen::Map<const Eigen::Vector<T, Eigen::Dynamic>> z(guess_state[0], n_rho+n_mom);
            Eigen::Map<Eigen::Vector<T, Eigen::Dynamic>> r(residual, n_rho+n_mom+2);
            auto rho = z(Eigen::seqN(0, n_rho));
            auto mom = z(Eigen::seqN(n_rho, n_mom));

            // build necessary objects for non-linear eq
            static auto Rt = RtStateOperator<T>(n_rho, n_mom, f, D);
            static auto effort = EffortStateVec<T>(n_rho, n_mom, temperature);
            effort.update_state(rho, mom);
            Rt.update_state(rho, mom);

            // solve non-linear eq and populate residual with result
            r = Jt.mat * effort.vec_t - Rt.mat * effort.vec_t + G.mat * input_vec;

            return true;
        }

        private:
            const int n_rho;
            const int n_mom;
            const double f;
            const double D;
            const double temperature;
            const Jt_operator& Jt;
            const G_operator& G;
            const Eigen::Vector2d& input_vec;
    };

    struct SteadyCompressorSystem{
        SteadyCompressorSystem(
            DiscreteNetwork<double>& network,
            const Eigen::Vector4d& input_vec
        );
        bool operator()(double const* const* guess_state, double* residual) const;
        private:
            DiscreteNetwork<double>& network;
            const Eigen::Vector4d& input_vec;
    };
}
