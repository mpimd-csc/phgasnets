# include "io.hpp"

namespace PHModel{

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
    Vector rho = pipe.rho;
    Vector mom = pipe.mom;

    auto h5path = "pipe" + std::to_string(counter++) + "/" + std::to_string(timetag);
    H5Easy::dump(file, h5path + "/density", rho);
    H5Easy::dump(file, h5path + "/pressure", rho*PHModel::GAS_CONSTANT*pipe.temperature);
    H5Easy::dump(file, h5path + "/momentum", mom);
    H5Easy::dump(file, h5path + "/timestamp", time);
  }
}

}