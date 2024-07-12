// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# pragma once

# include "derivative.hpp"
# include "gasconstant.hpp"

# include <vector>
# include <Eigen/Core>
# include <Eigen/SparseCore>
# include <ceres/jet.h>

namespace phgasnets {

  template<typename T>
  struct BaseOperator{

    protected:
      const int n_rho;
      const int n_mom;

    public:
    std::vector<Eigen::Triplet<T>> data;
    Eigen::SparseMatrix<T> mat;

    // Constructor
    BaseOperator(
        const int n_rho,
        const int n_mom
    ): n_rho(n_rho), n_mom(n_mom) {}

    // Update State
    void update_state(
      const Eigen::Ref<const Eigen::Vector<T, Eigen::Dynamic>>&,
      const Eigen::Ref<const Eigen::Vector<T, Eigen::Dynamic>>&
    ) {}

    // Update entry in the operator
    // WARN: Currently replaces only in existing non-zero entries
    void update_ij(const int i, const int j, const T value){
        for (int k = 0; k < data.size(); ++k)
            if (data[k].row() == i && data[k].col() == j) {
                data[k] = Eigen::Triplet<T>(i, j, value);
            }
        mat.coeffRef(i, j) = value;
    }

    // Destructor
    virtual ~BaseOperator() = default;

  };

  struct E_operator: BaseOperator<double>{
      E_operator(
          const int n_rho,
          const int n_mom
      );
  };

  struct Et_operator: BaseOperator<double>{
      Et_operator(
          const int n_rho,
          const int n_mom
      );
      private:
          const E_operator E;
  };

  struct U_operator: BaseOperator<double>{
      U_operator(
          const int n_rho,
          const int n_mom
      );
  };

  struct J_operator: BaseOperator<double>{
      J_operator(
          const int n_rho,
          const int n_mom,
          const double mesh_width
      );
      private:
          const double mesh_width;
  };

  struct Jt_operator: BaseOperator<double>{
      Jt_operator(
          const int n_rho,
          const int n_mom,
          const double mesh_width
      );
      private:
          const J_operator J;
          const U_operator U;
  };

  struct Y_operator: BaseOperator<double>{
      Y_operator(
          const int n_rho,
          const int n_mom
      );
  };

  template <typename T>
  struct R_operator: BaseOperator<T>{

    const double f;
    const double D;

    // Constructor
    R_operator(
        const int n_rho,
        const int n_mom,
        const double friction,
        const double diameter
    ) :
      BaseOperator<T>(n_rho, n_mom), f(friction), D(diameter)
    {
      this->data.resize(n_mom);
      this->mat.resize(n_rho+n_mom, n_rho+n_mom);
    }

    // (Overloaded) Update State
    void update_state(
        const Eigen::Ref<const Eigen::Vector<T, Eigen::Dynamic>>& rho,
        const Eigen::Ref<const Eigen::Vector<T, Eigen::Dynamic>>& mom
    ) {

        for (int i = 0; i < this->n_mom; ++i) {
          auto friction_term = ceres::abs(f * mom(i)/rho(i) / (2 * D));
          this->data[i] = Eigen::Triplet<T>(
            this->n_rho+i, this->n_rho+i, friction_term
          );
        }

        this->mat.setFromTriplets(
          this->data.begin(), this->data.end(), [] (const T&, const T& b) { return b; }
        );
    }
  }; // struct R_operator

  template <typename T>
  struct Rt_operator: BaseOperator<T> {

    private:
      R_operator<T> R;

    public:
    Rt_operator(
      const int n_rho,
      const int n_mom,
      const double friction,
      const double diameter
    ) :
        BaseOperator<T>(n_rho, n_mom),
        R(R_operator<T>(n_rho, n_mom, friction, diameter))
    {
        this->data.resize(n_mom);
        this->mat.resize(n_rho+n_mom+2, n_rho+n_mom+2);
    }

    void update_state(
      const Eigen::Ref<const Eigen::Vector<T, Eigen::Dynamic>>& rho,
      const Eigen::Ref<const Eigen::Vector<T, Eigen::Dynamic>>& mom
    ) {
      // Update the R_operator
      R.update_state(rho, mom);
      // Update self
      this->data = R.data;
      this->mat.setFromTriplets(
        this->data.begin(), this->data.end(), [] (const T&, const T& b) { return b; }
      );
    }
  }; // struct Rt_operator

  template <typename T>
  struct effortVec {
    private:
      const Y_operator Y;
      const int n_rho;
      const int n_mom;
      Eigen::Vector<T, Eigen::Dynamic> vec;

    public:
      double temperature;
      Eigen::Vector<T, Eigen::Dynamic> vec_t;

      effortVec(
        const int n_rho,
        const int n_mom,
        const double temperature
      ) :
      n_rho(n_rho), n_mom(n_mom), Y(Y_operator(n_rho, n_mom)),
      temperature(temperature),
      vec(n_rho+n_mom), vec_t(n_rho+n_mom+2)
      {}

      void update_state(
        const Eigen::Ref<const Eigen::Vector<T, Eigen::Dynamic>>& rho,
        const Eigen::Ref<const Eigen::Vector<T, Eigen::Dynamic>>& mom
      ){
        vec.segment(0, n_rho) = rho * phgasnets::GAS_CONSTANT * temperature;
        vec.segment(n_rho, n_mom) = mom;

        vec_t.segment(0, n_rho+n_mom) = vec;
        vec_t.segment(n_rho+n_mom, 2) = Y.mat * vec;
      }
  };

  template<typename T>
  struct G_operator: BaseOperator<T> {
    G_operator(
      const int n_rho,
      const int n_mom
    ) :
      BaseOperator<T>(n_rho, n_mom)
    {
      this->data.resize(2);
      this->data[0] = Eigen::Triplet<T>(n_rho+n_mom, 0, T(1.0));
      this->data[1] = Eigen::Triplet<T>(n_rho+n_mom+1, 1, T(1.0));

      this->mat.resize(n_rho+n_mom+2, 2);
      this->mat.setFromTriplets(this->data.begin(), this->data.end());
    }
  };
}
