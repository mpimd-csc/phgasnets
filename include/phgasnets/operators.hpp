// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

#pragma once
# include <unordered_map>
# include <memory>
# include <vector>
# include <Eigen/Core>
# include <Eigen/SparseCore>
# include "derivative.hpp"
# include "gasconstant.hpp"
using Vector = Eigen::VectorXd;
using SparseMatrix = Eigen::SparseMatrix<double>;
using Triplet = Eigen::Triplet<double>;

namespace PHModel {

    struct BasePHOperator{
        /**
         * Constructor for BasePHOperator.
         *
         * @param n_rho The number of rho.
         * @param n_mom The number of momentum.
         */
        BasePHOperator(
            const int n_rho,
            const int n_mom
        ): n_rho(n_rho), n_mom(n_mom) {}

        void update_state(const Vector&, const Vector&) {}

        // WARN: Currently replaces only in existing non-zero entries
        void update_ij(const int i, const int j, const double value){
            for (int k = 0; k < data.size(); ++k)
                if (data[k].row() == i && data[k].col() == j) {
                    data[k] = Triplet(i, j, value);
                }
            mat.coeffRef(i, j) = value;
        }

        virtual ~BasePHOperator() = default;

        protected:
            const int n_rho, n_mom;
        public:
            std::vector<Triplet> data;
            SparseMatrix mat;
    };

    struct E_operator: BasePHOperator{
        E_operator(
            const int n_rho,
            const int n_mom
        );
    };

    struct Et_operator: BasePHOperator{
        Et_operator(
            const int n_rho,
            const int n_mom
        );
        private:
            const E_operator E;
    };

    struct U_operator: BasePHOperator{
        U_operator(
            const int n_rho,
            const int n_mom
        );
    };

    struct J_operator: BasePHOperator{
        J_operator(
            const int n_rho,
            const int n_mom,
            const double mesh_width
        );
        private:
            const double mesh_width;
    };

    struct Jt_operator: BasePHOperator{
        Jt_operator(
            const int n_rho,
            const int n_mom,
            const double mesh_width
        );
        private:
            const J_operator J;
            const U_operator U;
    };

    struct R_operator: BasePHOperator{
        R_operator(
            const int n_rho,
            const int n_mom,
            const double friction,
            const double diameter
        );
        void update_state(
            const Vector& rho,
            const Vector& mom
        );
        public:
            const double f;
            const double D;
    };

    struct Rt_operator: BasePHOperator{
        Rt_operator(
            const int n_rho,
            const int n_mom,
            const double friction,
            const double diameter
        );

        void update_state(
            const Vector& rho,
            const Vector& mom
        );
        private:
            R_operator R;
    };

    struct Y_operator: BasePHOperator{
        Y_operator(
            const int n_rho,
            const int n_mom
        );
    };

    struct Effort{
        Effort(
            const int n_rho,
            const int n_mom,
            const double temperature
        );

        void update_state(
            const Vector& rho,
            const Vector& mom
        );

        private:
            const Y_operator Y;
            const int n_rho, n_mom;
            Vector vec;
        public:
            double temperature;
            Vector vec_t;
    };

    struct G_operator: BasePHOperator{
        G_operator(
            const int n_rho,
            const int n_mom
        );
    };

    Eigen::Vector2d input_vec(
        const double inlet_pressure,
        const double outlet_momentum
    );

} // namespace phgasnets
