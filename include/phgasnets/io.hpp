// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# pragma once

# include "network.hpp"

# include <highfive/H5Easy.hpp>

namespace phgasnets{

  struct NetworkStateWriter{
      NetworkStateWriter(const std::string& filename, const DiscreteNetwork<double>& network);
      void writeMesh();
      void writeState(const int& timetag, const int& time);
      private:
        H5Easy::File file;
        const DiscreteNetwork<double>& network;
  };

}
