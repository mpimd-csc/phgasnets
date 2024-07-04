// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# include <gasconstant.hpp>

double phgasnets::GAS_CONSTANT = 530.0;

double phgasnets::get_gas_constant(){
  return GAS_CONSTANT;
}

void phgasnets::set_gas_constant(const double new_gas_constant){
  GAS_CONSTANT = new_gas_constant;
}
