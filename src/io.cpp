// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# include "io.hpp"

namespace phgasnets{

NetworkStateWriter::NetworkStateWriter(
  const std::string& filename,
  const Network& network
) :
  file(filename, H5Easy::File::Truncate),
  network(network)
{}

void NetworkStateWriter::writeMesh() {
  int counter = 0;
  for(auto& pipe: network.pipes){
    auto h5path = "pipe" + std::to_string(counter++);
    H5Easy::dump(file, h5path + "/mesh", pipe.mesh);
  }
}

void NetworkStateWriter::writeState(const int& timetag, const int& time) {

  int counter = 0;
  for(auto& pipe: network.pipes){
    Eigen::VectorXd rho = pipe.rho;
    Eigen::VectorXd mom = pipe.mom;

    auto h5path = "pipe" + std::to_string(counter++) + "/" + std::to_string(timetag);
    H5Easy::dump(file, h5path + "/density", rho);
    H5Easy::dump(file, h5path + "/pressure", rho*phgasnets::GAS_CONSTANT*pipe.temperature);
    H5Easy::dump(file, h5path + "/momentum", mom);
    H5Easy::dump(file, h5path + "/timestamp", time);
  }
}

}
