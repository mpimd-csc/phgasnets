// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# pragma once

# include "operators.hpp"
# include "state_operators.hpp"
# include "pipe.hpp"
# include "compressor.hpp"
# include "network.hpp"
# include "utils.hpp"

# include <Eigen/SparseCore>
# include <nlohmann/json.hpp>

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
            const Network& network,
            const nlohmann::json& spatial_disc_params,
            const Eigen::Vector4d& input_vec
        ) : network(network), spatial_disc_params(spatial_disc_params), input_vec(input_vec)
        {}

        template <typename T>
        bool operator()(
            T const* const* guess_state,
            T* residual
        ) const {

            static auto discrete_network = discretize<T>(network, spatial_disc_params);

            Eigen::Map<const Eigen::Vector<T, Eigen::Dynamic>> z(guess_state[0], discrete_network.n_state);
            Eigen::Map<Eigen::Vector<T, Eigen::Dynamic>> r(residual, discrete_network.n_res);

            discrete_network.set_state(z);

            // solve non-linear eq and populate residual with result
            r = (
                discrete_network.J * discrete_network.effort
                - discrete_network.R * discrete_network.effort
                + discrete_network.G * input_vec
            );

            return true;
        }

        private:
            const Network& network;
            const nlohmann::json& spatial_disc_params;
            const Eigen::Vector4d& input_vec;
    };
}
