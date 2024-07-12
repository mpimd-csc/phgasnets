// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

#include "operators.hpp"

using namespace phgasnets;

E_operator::E_operator(
    const int n_rho,
    const int n_mom
) :
    BaseOperator<double>(n_rho, n_mom)
{
    data.resize(n_rho+n_mom);
    for (int i = 0; i < n_rho+n_mom; ++i)
        data[i] = Eigen::Triplet<double>(i, i, 1.0);

    mat.resize(n_rho+n_mom, n_rho+n_mom);
    mat.setFromTriplets(data.begin(), data.end());
}

Et_operator::Et_operator(
    const int n_rho,
    const int n_mom
) :
    BaseOperator<double>(n_rho, n_mom),
    E(E_operator(n_rho, n_mom))
{
    data = E.data;
    mat.resize(n_rho+n_mom+2, n_rho+n_mom);
    mat.setFromTriplets(data.begin(), data.end());
}

// U matrix constructor
U_operator::U_operator(
    const int n_rho,
    const int n_mom
) :
    BaseOperator<double>(n_rho, n_mom)
{
    data.resize(2);
    data[0] = Eigen::Triplet<double>(0, 0, 1.0);
    data[1] = Eigen::Triplet<double>(1, n_rho+n_mom-1, -1.0);

    mat.resize(2, n_rho+n_mom);
    mat.setFromTriplets(data.begin(), data.end());
}

// J matrix constructor
J_operator::J_operator(
    const int n_rho,
    const int n_mom,
    const double mesh_width
) :
   BaseOperator<double>(n_rho, n_mom), mesh_width(mesh_width)
{
    // Make two Dx triplets
    std::vector<Eigen::Triplet<double>> dx_1 = derivative_operator(n_rho, mesh_width);
    std::vector<Eigen::Triplet<double>> dx_2 = dx_1;

    // Block 1: minus Dx triplets with n_mom column offset
    for (auto& triplet : dx_1)
        triplet = Eigen::Triplet<double>(triplet.row(), n_mom+triplet.col(), -triplet.value());

    // Block 2 : Create minus Dx triplets with n_rho row offset
    for (auto& triplet : dx_2)
        triplet = Eigen::Triplet<double>(n_rho+triplet.row(), triplet.col(), -triplet.value());

    // Concatenate these triplets to form triplets for J operator
    data.resize(3*n_rho+3*n_mom);
    data.insert(data.end(), dx_1.begin(), dx_1.end());
    data.insert(data.end(), dx_2.begin(), dx_2.end());

    // Create Sparse Matrix
    mat.resize(n_rho+n_mom, n_rho+n_mom);
    mat.setFromTriplets(data.begin(), data.end());
}

// Jt_operator constructor
Jt_operator::Jt_operator(
    const int n_rho,
    const int n_mom,
    const double mesh_width
) :
    BaseOperator<double>(n_rho, n_mom),
    J(J_operator(n_rho, n_mom, mesh_width)),
    U(U_operator(n_rho, n_mom))
{
    // Add the J_operator triplets as is into Jt
    data.resize(3*n_rho+3*n_mom+2);
    data.insert(data.end(), J.data.begin(), J.data.end());

    // Add the U_operator triplets with row offset and negative value.
    for (auto& triplet : U.data)
        data.push_back(Eigen::Triplet<double>(n_rho+n_mom+triplet.row(), triplet.col(), -triplet.value()));

    // Create matrix operator
    mat.resize(n_rho+n_mom+2, n_rho+n_mom+2);
    mat.setFromTriplets(data.begin(), data.end());
}

// Y_operator constructor
Y_operator::Y_operator(
    const int n_rho,
    const int n_mom
) :
    BaseOperator<double>(n_rho, n_mom)
{
    data.resize(2);
    data[0] = Eigen::Triplet<double>(0, n_rho, 1.0);
    data[1] = Eigen::Triplet<double>(1, n_rho-1, 1.0);

    mat.resize(2, n_rho+n_mom);
    mat.setFromTriplets(data.begin(), data.end());
}
