# pragma once
# include "pipe.hpp"
# include "compressor.hpp"
# include "utils.hpp"
# include <vector>
# include <iostream>

namespace PHModel {
  struct Network {

    Network(std::vector<DiscretePipe>& pipes, std::vector<Compressor>& compressors);

    void set_gas_state(const Vector& state);

    Vector get_gas_state() const;

    public:
      std::vector<DiscretePipe>& pipes;
      std::vector<Compressor>& compressors;
      SparseMatrix E, J, R, G;
      Vector effort;
      int n_state, n_res;
  };
}