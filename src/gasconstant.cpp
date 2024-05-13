# include <gasconstant.hpp>

double PHModel::GAS_CONSTANT = 530.0;

double PHModel::get_gas_constant(){
  return GAS_CONSTANT;
}

void PHModel::set_gas_constant(const double new_gas_constant){
  GAS_CONSTANT = new_gas_constant;
}