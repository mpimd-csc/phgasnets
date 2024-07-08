// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# include <Eigen/SparseCore>
# include <nlohmann/json.hpp>
# include "operators.hpp"
# include "state_operators.hpp"
# include "network.hpp"
# include "utils.hpp"

namespace phgasnets {
    struct TransientSystem{
        TransientSystem(
            const int n_rho, const int n_mom,
            const Eigen::Ref<const Eigen::VectorXd>& current_state,
            const Et_operator& Et,
            const Jt_operator& Jt,
            const G_operator& G,
            const double& friction,
            const double& diameter,
            const double& temperature,
            const Eigen::Ref<const Eigen::Vector2d>& input_vec,
            const double time,
            const double timestep
        ):
            n_rho(n_rho), n_mom(n_mom),
            current_state(current_state),
            Et(Et), Jt(Jt), G(G), input_vec(input_vec),
            f(friction), D(diameter), temperature(temperature),
            time(time), timestep(timestep)
        {}

        template <typename T>
        bool operator()(T const* const* guess_state, T* residual) const {
            Eigen::Map<const Eigen::Vector<T, Eigen::Dynamic>> new_state(guess_state[0],n_rho+n_mom);
            Eigen::Map<Eigen::Vector<T, Eigen::Dynamic>> r(residual, n_rho+n_mom+2);

            // Implicit midpoint rule
            auto z = (new_state + current_state) * 0.5;
            auto dz_dt = (new_state - current_state) / timestep;

            // Distribute parameters into density and momentum vectors
            auto rho = z(Eigen::seqN(0, n_rho));
            auto mom = z(Eigen::seqN(n_rho, n_mom));

            // build necessary objects for non-linear eq
            static auto Rt = RtStateOperator<T>(n_rho, n_mom, f, D);
            static auto effort = EffortStateVec<T>(n_rho, n_mom, temperature);
            // Update effort and Rt_mat
            effort.update_state(rho, mom);
            Rt.update_state(rho, mom);

            // solve non-linear eq and populate residual with result
            r = Et.mat * dz_dt - Jt.mat * effort.vec_t + Rt.mat * effort.vec_t - G.mat * input_vec;

            return true;
        }

        private:
            const int n_rho, n_mom;
            const Eigen::VectorXd current_state;
            const Et_operator& Et;
            const Jt_operator& Jt;
            const G_operator& G;
            const double f;
            const double D;
            const double temperature;
            const Eigen::Vector2d input_vec;
            const double time;
            const double timestep;
    };

    struct TransientCompressorSystem{
        TransientCompressorSystem(
            const Network& network,
            const nlohmann::json& disc_params,
            const Eigen::Ref<const Eigen::VectorXd>& current_state,
            const Eigen::Ref<const Eigen::Vector4d>& input_vec,
            const double time
        ):
            network(network), current_state(current_state), disc_params(disc_params),
            input_vec(input_vec), time(time), timestep(disc_params["time"]["step"])
        {}

        template <typename T>
        bool operator()(
            T const* const* guess_state,
            T* residual
        ) const {
            static auto discrete_network = discretize<T>(network, disc_params["space"]);

            Eigen::Map<const Eigen::Vector<T, Eigen::Dynamic>> new_state(guess_state[0], discrete_network.n_state);
            Eigen::Map<Eigen::Vector<T, Eigen::Dynamic>> r(residual, discrete_network.n_res);

            // Implicit midpoint rule
            auto z = (new_state + current_state) * 0.5;
            auto dz_dt = (new_state - current_state) / timestep;

            discrete_network.set_state(z);

            r = (
                discrete_network.E * dz_dt
                - discrete_network.J * discrete_network.effort
                + discrete_network.R * discrete_network.effort
                - discrete_network.G * input_vec
            );

            return true;
        }

        private:
            const Network& network;
            const nlohmann::json& disc_params;
            const Eigen::VectorXd current_state;
            const Eigen::Vector4d input_vec;
            const double time;
            const double timestep;
    };
}
