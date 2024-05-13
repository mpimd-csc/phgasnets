#include "operators.hpp"

using namespace PHModel;

E_operator::E_operator(
    const int n_rho,
    const int n_mom
) :
    BasePHOperator(n_rho, n_mom)
{
    data.resize(n_rho+n_mom);
    for (int i = 0; i < n_rho+n_mom; ++i)
        data[i] = Triplet(i, i, 1.0);

    mat.resize(n_rho+n_mom, n_rho+n_mom);
    mat.setFromTriplets(data.begin(), data.end());
}

Et_operator::Et_operator(
    const int n_rho,
    const int n_mom
) :
    BasePHOperator(n_rho, n_mom),
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
    BasePHOperator(n_rho, n_mom)
{
    data.resize(2);
    data[0] = Triplet(0, 0, 1.0);
    data[1] = Triplet(1, n_rho+n_mom-1, -1.0);

    mat.resize(2, n_rho+n_mom);
    mat.setFromTriplets(data.begin(), data.end());
}

// J matrix constructor
J_operator::J_operator(
    const int n_rho,
    const int n_mom,
    const double mesh_width
) :
   BasePHOperator(n_rho, n_mom), mesh_width(mesh_width)
{
    // Make two Dx triplets
    std::vector<Triplet> dx_1 = derivative_operator(n_rho, mesh_width);
    std::vector<Triplet> dx_2 = dx_1;

    // Block 1: minus Dx triplets with n_mom column offset
    for (auto& triplet : dx_1)
        triplet = Triplet(triplet.row(), n_mom+triplet.col(), -triplet.value());

    // Block 2 : Create minus Dx triplets with n_rho row offset
    for (auto& triplet : dx_2)
        triplet = Triplet(n_rho+triplet.row(), triplet.col(), -triplet.value());

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
    BasePHOperator(n_rho, n_mom),
    J(J_operator(n_rho, n_mom, mesh_width)),
    U(U_operator(n_rho, n_mom))
{
    // Add the J_operator triplets as is into Jt
    data.resize(3*n_rho+3*n_mom+2);
    data.insert(data.end(), J.data.begin(), J.data.end());

    // Add the U_operator triplets with row offset and negative value.
    for (auto& triplet : U.data)
        data.push_back(Triplet(n_rho+n_mom+triplet.row(), triplet.col(), -triplet.value()));

    // Create matrix operator
    mat.resize(n_rho+n_mom+2, n_rho+n_mom+2);
    mat.setFromTriplets(data.begin(), data.end());
}

// R Operator constructor
R_operator::R_operator(
    const int n_rho,
    const int n_mom,
    const double friction,
    const double diameter
) :
    BasePHOperator(n_rho, n_mom),
    f(friction), D(diameter)
{
    data.resize(n_mom);
    mat.resize(n_rho+n_mom, n_rho+n_mom);
}

void R_operator::update_state(
    const Vector& rho,
    const Vector& mom
) {
    Vector friction_term = (f * (mom.array()/rho.array()).abs()) / (2 * D);

    for (int i = 0; i < n_mom; ++i)
        data[i] = Triplet(n_rho+i, n_rho+i, friction_term(i));

    mat.setFromTriplets(data.begin(), data.end(), [] (const int&,const int& b) { return b; });
}

// Rt Operator constructor
Rt_operator::Rt_operator(
    const int n_rho,
    const int n_mom,
    const double friction,
    const double diameter
) :
    BasePHOperator(n_rho, n_mom),
    R(R_operator(n_rho, n_mom, friction, diameter))
{
    data.resize(n_mom);
    mat.resize(n_rho+n_mom+2, n_rho+n_mom+2);
}


/**
 * Updates the state of Rt_operator based on the input rho and mom vectors.
 *
 * @param rho The vector containing density values.
 * @param mom The vector containing momentum values.
 *
 * @throws None
 */
void Rt_operator::update_state(
    const Vector& rho,
    const Vector& mom
) {
    // Update the R_operator
    R.update_state(rho, mom);
    // Update Rt operator
    data = R.data;
    mat.setFromTriplets(data.begin(), data.end(), [] (const int&,const int& b) { return b; });
}

Rt_operator2::Rt_operator2(
    const int n_rho,
    const int n_mom,
    const double friction,
    const double diameter
) :
    BasePHOperator(n_rho, n_mom),
    friction(friction), diameter(diameter),
    friction_term_vec(n_mom), I(n_mom, n_mom), V(n_mom, n_rho+n_mom+2)
{
    data.resize(n_mom);
    mat.resize(n_rho+n_mom+2, n_rho+n_mom+2);

    // Create the V and I matrices
    // V = [0 I 0]
    std::vector<Triplet> data_v(n_mom);
    for (int i = 0; i < n_mom; ++i)
        data_v[i] = Triplet(i, n_rho+i, 1.0);
    V.setFromTriplets(data_v.begin(), data_v.end());

    I.setIdentity();
}

void Rt_operator2::update_state(
    const Vector& rho,
    const Vector& mom
) {
    // Update the friction term
    friction_term_vec = (friction * (mom.array()/rho.array()).abs()) / (2 * diameter);

    for(int i = 0; i < n_mom; ++i){
        data[i] = Triplet(n_rho+i, n_rho+i, friction_term_vec(i));
    }
    mat = V.transpose() * (I * friction_term_vec) * V;
}

// Y_operator constructor
Y_operator::Y_operator(
    const int n_rho,
    const int n_mom
) :
    BasePHOperator(n_rho, n_mom)
{
    data.resize(2);
    data[0] = Triplet(0, n_rho, 1.0);
    data[1] = Triplet(1, n_rho-1, 1.0);

    mat.resize(2, n_rho+n_mom);
    mat.setFromTriplets(data.begin(), data.end());
}

Effort::Effort(
    const int n_rho,
    const int n_mom,
    const double temperature
) :
    n_rho(n_rho), n_mom(n_mom), Y(Y_operator(n_rho, n_mom)),
    temperature(temperature),
    vec(n_rho+n_mom), vec_t(n_rho+n_mom+2)
{}

void Effort::update_state(
    const Vector& rho,
    const Vector& mom
) {
    vec.segment(0, n_rho) = rho * PHModel::GAS_CONSTANT * temperature;
    vec.segment(n_rho, n_mom) = mom;

    vec_t.segment(0, n_rho+n_mom) = vec;
    vec_t.segment(n_rho+n_mom, 2) = Y.mat * vec;
}

G_operator::G_operator(
    const int n_rho,
    const int n_mom
) :
    BasePHOperator(n_rho, n_mom)
{
    data.resize(2);
    data[0] = Triplet(n_rho+n_mom, 0, 1.0);
    data[1] = Triplet(n_rho+n_mom+1, 1, 1.0);

    mat.resize(n_rho+n_mom+2, 2);
    mat.setFromTriplets(data.begin(), data.end());
}

Eigen::Vector2d PHModel::input_vec(
    const double inlet_pressure,
    const double outlet_momentum
) {
    return Eigen::Vector2d {{ inlet_pressure, -outlet_momentum}};
}
