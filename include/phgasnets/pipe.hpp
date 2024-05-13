# pragma once

# include <Eigen/Core>
# include <Eigen/SparseCore>
# include "operators.hpp"
# include <vector>
# include <memory>
# include <unordered_map>

using Vector2d = Eigen::Vector2d;
using Vector = Eigen::VectorXd;
using SparseMatrix = Eigen::SparseMatrix<double>;


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

    void set_gas_state(const Vector& new_state);

    void set_density(Vector& new_density);

    void set_pressure(Vector& new_pressure);

    void set_pressure(double new_pressure);

    void set_momentum(Vector& new_momentum);

    void set_momentum(double new_momentum);

    void set_temperature(float new_temperature);

    Vector get_gas_state() const;

    Vector get_density() const;

    Vector get_pressure() const;

    Vector get_momentum() const;

    public:
      const int n_x, n_rho, n_mom, n_res, n_state;
      const float length, diameter, friction, mesh_width;
      float temperature;
      Vector mesh, rho, mom;
      Et_operator Et; Rt_operator Rt; Jt_operator Jt;
      Effort effort; G_operator G;
      Vector2d input_vec;
  };
}