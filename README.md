# Code supplement for "Modelling Gas Networks with Compressors: A Port-Hamiltonian Approach"

Supplement to the article submitted for PAMM 2024.

Authors:
  - [Ashwin S. Nayak](https://orcid.org/0000-0002-9855-2377)
  - [Thomas Bendokat](https://orcid.org/0000-0002-0671-6291)

Affiliation:
  - [Max Planck Institute for Dynamics of Complex Technical Systems](https://www.mpi-magdeburg.mpg.de/)

License:
  - GNU GPL v3, see [`LICENSE.md`](LICENSE.md).

## Prerequisites

Building phgasnets requires the following software installed:

* A C++17-compliant compiler
* CMake `>= 3.9`
* [Eigen](https://gitlab.com/libeigen/eigen)
* [Ceres](http://ceres-solver.org/)
* [HighFive](https://bluebrain.github.io/HighFive/)
* [nlohmann_json](https://github.com/nlohmann/json)


## Development Container

You can make use of [development containers](https://containers.dev/) to develop and also test the codes in this repository.

Necessary tools :

- [Docker](https://docs.docker.com/engine/install/) installed and configured to access the Hinkskalle container registry for MPI-Magdeburg CSC Group.
- [Visual Studio Code](https://code.visualstudio.com/) with [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) extension.


## Building phgasnets

A convenience script [`RUNME.sh`](README.md) is provided to build and run the demos.

The following sequence of commands builds phgasnets.
It assumes that your current working directory is the top-level directory [phgasnets.cpp] and the build files will be placed in `build` directory.

```
cmake -B build -S . -DCMAKE_BUILD_TYPE="Release"
```

If any of the dependencies are at a custom location and CMake cannot find it, you may indicate the paths as,

```
cmake -B build -S . -DCMAKE_BUILD_TYPE="Release" -DCMAKE_PREFIX_PATH="\path\to\custom\library\location"
```

To compile in debug mode set the `DCMAKE_BUILD_TYPE=` flag to `Debug` instead.
