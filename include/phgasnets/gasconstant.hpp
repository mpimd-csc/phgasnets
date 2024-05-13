#pragma once

namespace PHModel {

  extern double GAS_CONSTANT;

  /**
   * Retrieves the value of the gas constant.
   *
   * @return The value of the gas constant.
   */
  double get_gas_constant();

  /**
   * Sets the gas constant to the specified value.
   *
   * @param new_gas_constant the new value for the gas constant
   */
  void set_gas_constant(const double new_gas_constant);

}
