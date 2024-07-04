// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# include "pipe.hpp"
# include "operators.hpp"

namespace phgasnets{

  DiscretePipe::DiscretePipe(
    const float length,
    const float diameter,
    const float friction,
    const float temperature,
    const int nx
  ) :
    length(length), diameter(diameter), friction(friction),
    n_x(nx), mesh_width(length/nx),  mesh(nx+1),
    n_rho(nx+1), n_mom(nx+1), n_res(2*nx+4), n_state(2*nx+2),
    rho(n_x+1), mom(nx+1),
    temperature(temperature),
    Et(Et_operator(n_x+1, n_x+1)),
    Jt(Jt_operator(n_x+1, n_x+1, mesh_width)),
    Rt(Rt_operator(n_x+1, n_x+1, friction, diameter)),
    effort(Effort(n_rho, n_mom, temperature)),
    G(G_operator(n_x+1, n_x+1))
  {
    mesh = Eigen::VectorXd::LinSpaced(n_x+1, 0.0, length);
  }

  void DiscretePipe::set_gas_state(const Eigen::Ref<const Eigen::VectorXd>& new_state){
    rho = new_state(Eigen::seqN(0, n_rho));
    mom = new_state(Eigen::seqN(n_rho, n_mom));

    // Update state dependent operators
    Rt.update_state(rho, mom);
    effort.update_state(rho, mom);
  }

  void DiscretePipe::set_density(const Eigen::Ref<const Eigen::VectorXd>& new_density){
    rho = new_density;
  }

  void DiscretePipe::set_pressure(const Eigen::Ref<const Eigen::VectorXd>& new_pressure){
    rho = new_pressure/(phgasnets::GAS_CONSTANT*temperature);
  }

  void DiscretePipe::set_pressure(double new_pressure){
    rho.setConstant(new_pressure/(phgasnets::GAS_CONSTANT*temperature));
  }

  void DiscretePipe::set_momentum(const Eigen::Ref<const Eigen::VectorXd>& new_momentum){
    mom = new_momentum;
  }

  void DiscretePipe::set_momentum(double new_momentum){
    mom.setConstant(new_momentum);
  }

  void DiscretePipe::set_temperature(float new_temperature){
    temperature = new_temperature;
    // set temperature to any associated children
    effort.temperature = new_temperature;
  }

  Eigen::VectorXd DiscretePipe::get_gas_state() const {
    Eigen::VectorXd state(rho.size()+mom.size());
    state << rho, mom;
    return state;
  }

  Eigen::VectorXd DiscretePipe::get_density() const {
    return rho;
  }

  Eigen::VectorXd DiscretePipe::get_momentum() const {
    return mom;
  }

  Eigen::VectorXd DiscretePipe::get_pressure() const {
    return rho * phgasnets::GAS_CONSTANT*temperature;
  }
}
