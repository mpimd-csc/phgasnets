
            _                                      _        
           | |                                    | |       
     _ __  | |__    __ _   __ _  ___  _ __    ___ | |_  ___ 
    | '_ \ | '_ \  / _` | / _` |/ __|| '_ \  / _ \| __|/ __|
    | |_) || | | || (_| || (_| |\__ \| | | ||  __/| |_ \__ \
    | .__/ |_| |_| \__, | \__,_||___/|_| |_| \___| \__||___/
    | |             __/ |                                   
    |_|            |___/                                    

phgasnets-cpp
=============

Code supplement for "Modelling Gas Networks with Compressors: A Port-Hamiltonian Approach"

Authors:
  - [Ashwin S. Nayak](https://orcid.org/0000-0002-9855-2377)
  - [Thomas Bendokat](https://orcid.org/0000-0002-0671-6291)

Affiliation:
  - [Max Planck Institute for Dynamics of Complex Technical Systems](https://www.mpi-magdeburg.mpg.de/), Magdeburg, Germany.

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

- [Docker](https://docs.docker.com/engine/install/),
- [Visual Studio Code](https://code.visualstudio.com/) with [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) extension.


## Building phgasnets

A convenience script [`RUNME.sh`](RUNME.sh) is provided to build and run all the demos.

If you choose to pursue it differently, the following sequence of commands builds phgasnets.

> Current working directory is assumed as the top-level project directory and the build files will be placed in `build` directory.

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE="Release"
```

If any of the dependencies are at a custom location and CMake cannot find it, you may indicate the paths as,

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE="Release" -DCMAKE_PREFIX_PATH="\path\to\custom\library\location"
```

To compile in debug mode set the `DCMAKE_BUILD_TYPE=` flag to `Debug` instead.
