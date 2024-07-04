// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# pragma once

#include <string>
#include <memory>
#include <map>
#include <functional>
#include <variant>

namespace phgasnets {
  struct Compressor {
    /**
     * Constructs a Compressor object with the specified compression ratio and isentropic exponent.
     *
     * @param type the framework type of the compressor
     * @param model the model assumption of the compressor
     * @param specification the fixed spec of the compressor (compression_ratio/outlet_pressure)
     * @param isentropic_exponent the isentropic exponent of the fluid
     *
     * @throws None
     */
      Compressor(
        const std::string type,
        const std::string model,
        const double specification,
        const double isentropic_exponent
      );
      void update_compression_ratio(double new_compression_ratio);
      public:
        const std::string type; // "FC type or FP type
        const std::string model; // "AV" model or "AM" model
        double specification; // compression_ratio OR outlet pressure
        double compression_ratio;
        const double isentropic_exponent; // specific heat ratio of fluid
        double temperature_scale, momentum_scale, pressure_scale, density_scale;
  };

}
