// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# include "network.hpp"
# include <highfive/H5Easy.hpp>

namespace PHModel{

  struct NetworkStateWriter{
      NetworkStateWriter(const std::string& filename, const Network& network);
      void writeMesh();
      void writeState(const int& timetag, const int& time);
      private:
        H5Easy::File file;
        const Network& network;
  };

}