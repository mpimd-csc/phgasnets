// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# pragma once

# include <Eigen/Core>
# include <Eigen/SparseCore>
# include <nlohmann/json.hpp>
# include "operators.hpp"
# include "state_operators.hpp"
# include <vector>
# include <memory>
# include <unordered_map>

namespace phgasnets{

  struct Pipe{

    Pipe(
      const float length,
      const float diameter,
      const float friction,
      const float temperature
    ): length(length), diameter(diameter), friction(friction), temperature(temperature)
    {}

    Pipe(
      const nlohmann::json& params,
      const float temperature
    ): Pipe(params["length"], params["diameter"], params["friction"], temperature)
    {}

  public:
    const float length;
    const float diameter;
    const float friction;
    float temperature;

  };

  template <typename T>
  struct DiscretePipe{

    DiscretePipe(
      const float length,
      const float diameter,
      const float friction,
      const float temperature,
      const int nx
    ):
    length(length), diameter(diameter), friction(friction),
    n_x(nx), mesh_width(length/nx),  mesh(nx+1),
    n_rho(nx+1), n_mom(nx+1), n_res(2*nx+4), n_state(2*nx+2),
    rho(n_x+1), mom(nx+1),
    temperature(temperature),
    Et(Et_operator(n_x+1, n_x+1)),
    Jt(Jt_operator(n_x+1, n_x+1, mesh_width)),
    Rt(RtStateOperator<T>(n_x+1, n_x+1, friction, diameter)),
    effort(EffortStateVec<T>(n_rho, n_mom, temperature)),
    G(GStateOperator<T>(n_x+1, n_x+1))
    {
      mesh = Eigen::VectorXd::LinSpaced(n_x+1, 0.0, length);
    }

    DiscretePipe(
      const Pipe& pipe,
      const int nx
    ): DiscretePipe(pipe.length, pipe.diameter, pipe.friction, pipe.temperature, nx)
    {}

    virtual ~DiscretePipe() = default; // Destructor

    void set_state(const Eigen::Ref<const Eigen::Vector<T, Eigen::Dynamic>>& new_state){
      rho = new_state(Eigen::seqN(0, n_rho));
      mom = new_state(Eigen::seqN(n_rho, n_mom));

      // Update state dependent operators
      Rt.update_state(rho, mom);
      effort.update_state(rho, mom);
    }

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
      Eigen::VectorXd mesh;
      Eigen::Vector<T, Eigen::Dynamic> rho, mom;
      Et_operator Et;
      RtStateOperator<T> Rt;
      Jt_operator Jt;
      EffortStateVec<T> effort;
      GStateOperator<T> G;
  };

}
