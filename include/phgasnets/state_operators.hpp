# pragma once

# include <vector>
# include <Eigen/Core>
# include <Eigen/SparseCore>
# include "operators.hpp"
# include <ceres/jet.h>

namespace PHModel{

  template<typename T>
  struct BaseStateOperator{

    protected:
      const int n_rho;
      const int n_mom;

    public:
    std::vector<Eigen::Triplet<T>> data;
    Eigen::SparseMatrix<T> mat;

    // Constructor
    BaseStateOperator(
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
    virtual ~BaseStateOperator() = default;

  };

  template <typename T>
  struct RStateOperator: BaseStateOperator<T>{

    const double f;
    const double D;

    // Constructor
    RStateOperator(
        const int n_rho,
        const int n_mom,
        const double friction,
        const double diameter
    ) :
      BaseStateOperator<T>(n_rho, n_mom), f(friction), D(diameter)
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
  }; // struct RStateOperator

  template <typename T>
  struct RtStateOperator: BaseStateOperator<T> {

    private:
      RStateOperator<T> R;

    public:
    RtStateOperator(
      const int n_rho,
      const int n_mom,
      const double friction,
      const double diameter
    ) :
        BaseStateOperator<T>(n_rho, n_mom),
        R(RStateOperator<T>(n_rho, n_mom, friction, diameter))
    {
        this->data.resize(n_mom);
        this->mat.resize(n_rho+n_mom+2, n_rho+n_mom+2);
    }

    void update_state(
      const Eigen::Ref<const Eigen::Vector<T, Eigen::Dynamic>>& rho,
      const Eigen::Ref<const Eigen::Vector<T, Eigen::Dynamic>>& mom
    ) {
      // Update the RStateOperator
      R.update_state(rho, mom);
      // Update self
      this->data = R.data;
      this->mat.setFromTriplets(
        this->data.begin(), this->data.end(), [] (const T&, const T& b) { return b; }
      );
    }
  }; // struct RtStateOperator

  template <typename T>
  struct EffortStateVec {
    private:
      const Y_operator Y;
      const int n_rho;
      const int n_mom;
      Eigen::Vector<T, Eigen::Dynamic> vec;

    public:
      double temperature;
      Eigen::Vector<T, Eigen::Dynamic> vec_t;

      EffortStateVec(
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
        vec.segment(0, n_rho) = rho * PHModel::GAS_CONSTANT * temperature;
        vec.segment(n_rho, n_mom) = mom;

        vec_t.segment(0, n_rho+n_mom) = vec;
        vec_t.segment(n_rho+n_mom, 2) = Y.mat * vec;
      }
  };

  template<typename T>
  struct GStateOperator: BaseStateOperator<T> {
    GStateOperator(
      const int n_rho,
      const int n_mom
    ) :
      BaseStateOperator<T>(n_rho, n_mom)
    {
      this->data.resize(2);
      this->data[0] = Eigen::Triplet<T>(n_rho+n_mom, 0, T(1.0));
      this->data[1] = Eigen::Triplet<T>(n_rho+n_mom+1, 1, T(1.0));

      this->mat.resize(n_rho+n_mom+2, 2);
      this->mat.setFromTriplets(this->data.begin(), this->data.end());
    }
  };

} // namespace PHModel