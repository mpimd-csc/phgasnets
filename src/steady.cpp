// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# include "steady.hpp"

PHModel::SteadySystem::SteadySystem(
    const int n_rho, const int n_mom,
    const Jt_operator& Jt,
    Rt_operator& Rt,
    Effort& effort,
    const G_operator& G,
    const Vector2d& input_vec
) :
    n_rho(n_rho), n_mom(n_mom),
    Jt(Jt), Rt(Rt), effort(effort), G(G), input_vec(input_vec)
{}


bool PHModel::SteadySystem::operator()(
    double const* const* guess_state,
    double* residual
) const {
    // Distribute parameters into density and momentum vectors
    Eigen::Map<const Vector> z(guess_state[0],n_rho+n_mom);
    Eigen::Map<Vector> r(residual, n_rho+n_mom+2);

    Vector rho = z(Eigen::seqN(0, n_rho));
    Vector mom = z(Eigen::seqN(n_rho, n_mom));

    // build necessary objects for non-linear eq
    effort.update_state(rho, mom);
    Rt.update_state(rho, mom);

    // solve non-linear eq and populate residual with result
    r = (Jt.mat - Rt.mat) * effort.vec_t + G.mat * input_vec;

    return true;
}

PHModel::SteadyCompressorSystem::SteadyCompressorSystem(
    Network& network,
    const Eigen::Vector4d& input_vec
) :
  network(network), input_vec(input_vec)
{}

bool PHModel::SteadyCompressorSystem::operator()(
    double const* const* guess_state,
    double* residual
) const {
    Eigen::Map<const Vector> z(guess_state[0],network.n_state);
    Eigen::Map<Vector> r(residual, network.n_res);

    network.set_gas_state(z);

    // solve non-linear eq and populate residual with result
    r = (network.J - network.R) * network.effort + network.G * input_vec;

    return true;
}
