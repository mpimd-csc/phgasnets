# pragma once

#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <functional>
#include <variant>

namespace PHModel {
  struct Compressor {
    /**
     * Constructs a Compressor object with the specified compression ratio and isentropic exponent.
     *
     * @param compression_ratio the compression ratio of the compressor
     * @param isentropic_exponent the isentropic exponent of the compressor
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
