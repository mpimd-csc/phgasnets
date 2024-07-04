// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# include "steady.hpp"

phgasnets::SteadyCompressorSystem::SteadyCompressorSystem(
    Network& network,
    const Eigen::Vector4d& input_vec
) :
  network(network), input_vec(input_vec)
{}


bool phgasnets::SteadyCompressorSystem::operator()(
    double const* const* guess_state,
    double* residual
) const {
    Eigen::Map<const Eigen::VectorXd> z(guess_state[0],network.n_state);
    Eigen::Map<Eigen::VectorXd> r(residual, network.n_res);

    network.set_gas_state(z);

    // solve non-linear eq and populate residual with result
    r = (network.J - network.R) * network.effort + network.G * input_vec;

    return true;
}
