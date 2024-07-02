// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# include "transient.hpp"

PHModel::TransientCompressorSystem::TransientCompressorSystem(
    Network& network,
    const Eigen::Ref<const Eigen::VectorXd>& current_state,
    const Eigen::Ref<const Eigen::Vector4d>& input_vec,
    const double time,
    const double timestep
) :
    network(network), current_state(current_state),
    input_vec(input_vec), time(time), timestep(timestep)
{}

bool PHModel::TransientCompressorSystem::operator()(
    double const* const* guess_state,
    double* residual
) const {
    Eigen::Map<const Eigen::VectorXd> new_state(guess_state[0], network.n_state);
    Eigen::Map<Eigen::VectorXd> r(residual, network.n_res);

    // Implicit midpoint rule
    Eigen::VectorXd z = (new_state + current_state) * 0.5;
    Eigen::VectorXd dz_dt = (new_state - current_state) / timestep;

    network.set_gas_state(z);

    r = network.E * dz_dt - (network.J - network.R) * network.effort - network.G * input_vec;

    return true;
}
