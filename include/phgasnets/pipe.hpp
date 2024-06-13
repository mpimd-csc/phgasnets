// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# pragma once

# include <Eigen/Core>
# include <Eigen/SparseCore>
# include "operators.hpp"
# include <vector>
# include <memory>
# include <unordered_map>

namespace PHModel{

  struct DiscretePipe{

    DiscretePipe(
      const float length,
      const float diameter,
      const float friction,
      const float temperature,
      const int Nx
    ); // Constructor

    virtual ~DiscretePipe() = default; // Destructor

    void set_gas_state(const Eigen::VectorXd& new_state);

    void set_density(Eigen::VectorXd& new_density);

    void set_pressure(Eigen::VectorXd& new_pressure);

    void set_pressure(double new_pressure);

    void set_momentum(Eigen::VectorXd& new_momentum);

    void set_momentum(double new_momentum);

    void set_temperature(float new_temperature);

    Eigen::VectorXd get_gas_state() const;

    Eigen::VectorXd get_density() const;

    Eigen::VectorXd get_pressure() const;

    Eigen::VectorXd get_momentum() const;

    public:
      const int n_x;
      const int n_rho;
      const int n_mom;
      const int n_res;
      const int n_state;
      const float length;
      const float diameter;
      const float friction;
      const float mesh_width;
      float temperature;
      Eigen::VectorXd mesh, rho, mom;
      Et_operator Et;
      Rt_operator Rt;
      Jt_operator Jt;
      Effort effort;
      G_operator G;
      Eigen::Vector2d input_vec;
  };
}
