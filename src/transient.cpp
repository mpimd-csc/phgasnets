// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# include "transient.hpp"

PHModel::TransientSystem::TransientSystem(
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
) :
    n_rho(n_rho), n_mom(n_mom),
    current_state(current_state),
    Et(Et), Jt(Jt),  Rt(Rt), effort(effort), G(G), input_vec(input_vec),
    time(time), timestep(timestep)
{}

bool PHModel::TransientSystem::operator()(
    double const* const* guess_state,
    double* residual
) const
{
    Eigen::Map<const Vector> new_state(guess_state[0],n_rho+n_mom);
    Eigen::Map<Vector> r(residual, n_rho+n_mom+2);

    // Implicit midpoint rule
    Vector z = (new_state + current_state) * 0.5;
    Vector dz_dt = (new_state - current_state) / timestep;

    // Distribute parameters into density and momentum vectors
    Vector rho = z(Eigen::seqN(0, n_rho));
    Vector mom = z(Eigen::seqN(n_rho, n_mom));

    // Update effort and Rt_mat
    effort.update_state(rho, mom);
    Rt.update_state(rho, mom);

    // solve non-linear eq and populate residual with result
    r = Et.mat * dz_dt - (Jt.mat - Rt.mat) * effort.vec_t - G.mat * input_vec;

    return true;
}

PHModel::TransientCompressorSystem::TransientCompressorSystem(
    Network& network,
    const Vector& current_state,
    const Eigen::Vector4d& input_vec,
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
    Eigen::Map<const Vector> new_state(guess_state[0], network.n_state);
    Eigen::Map<Vector> r(residual, network.n_res);

    // Implicit midpoint rule
    Vector z = (new_state + current_state) * 0.5;
    Vector dz_dt = (new_state - current_state) / timestep;

    network.set_gas_state(z);

    r = network.E * dz_dt - (network.J - network.R) * network.effort - network.G * input_vec;

    return true;
}
