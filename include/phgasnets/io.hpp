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