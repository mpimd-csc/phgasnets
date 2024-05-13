# include "compressor.hpp"
# include <cmath>

namespace PHModel {

Compressor::Compressor(
  std::string type,
  std::string model,
  double specification,
  const double isentropic_exponent
):
  type(type), model(model),
  specification(specification),
  isentropic_exponent(isentropic_exponent),
  compression_ratio(1.0),
  temperature_scale(1.0),
  momentum_scale(1.0),
  density_scale(1.0),
  pressure_scale(1.0)
{
  // Compression Ratio Specification
    if (type == "FC") {
      this->update_compression_ratio(specification);
    }
}

void Compressor::update_compression_ratio(double new_compression_ratio) {
  compression_ratio = new_compression_ratio;
  temperature_scale = std::pow(compression_ratio, (isentropic_exponent-1)/isentropic_exponent);
  momentum_scale = std::pow(compression_ratio, 1/isentropic_exponent);
  density_scale = std::pow(compression_ratio, 1/isentropic_exponent);
  pressure_scale = compression_ratio;
}

}