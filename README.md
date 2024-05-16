
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

## Getting Started

To use the code, you must first set the environment and dependent libraries.

You may either choose to use,
  - Development containers
  - Manually configure and build.

Once the library is built, you can **run the demos** provided in the `demos/` folder.

Three demos are provided:

  - `single_pipe` demo runs a transient simulation of the Yamal-Europe pipeline configuration (without a compressor).
  - `two_pipe_compressor` demo runs the Yamal-Europe pipeline configuration with a FC-AV compressor (CR=1.2) placed midway.
  - `four_compressor_types` demo runs the Yamal-Europe pipeline configuration with all four types of compressors placed midway.


### Development Container

You can make use of [development containers](https://containers.dev/) to set up the environment, run the demos and also develop the codes in this repository.

Necessary tools :

- [Docker](https://docs.docker.com/engine/install/),
- [Visual Studio Code](https://code.visualstudio.com/) with [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) extension.


### Manually configure and build

If you are not using the development container, you need to manually build `phgasnets` library.

Building requires the following dependencies:

* A C++17-compliant compiler
* CMake `>= 3.9`
* [Eigen](https://gitlab.com/libeigen/eigen) for handling linear algebra,
* [Ceres](http://ceres-solver.org/) for solving non-linear system of equations,
* [HDF5](https://www.hdfgroup.org/solutions/hdf5/) and [HighFive](https://bluebrain.github.io/HighFive/) for writing/reading states to HDF5 format,
* [nlohmann_json](https://github.com/nlohmann/json) for reading JSON configuration files.

Note the locations of these libraries in case they are not installed through standard package managers.

The following sequence of commands builds phgasnets.

> Current working directory is assumed as the top-level project directory and the build files will be placed in `build` directory.

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE="Release"
```

CMake looks for the dependencies in standard UNIX paths, but if any of the dependencies are at a custom location the paths may be indicated as,

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE="Release" -DCMAKE_PREFIX_PATH="/path/to/custom/library1;/path/to/custom/library2"
```

To compile in debug mode set `DCMAKE_BUILD_TYPE=Debug` instead.

### Run Demos

The demo executables are available in the `build` directory and take configuration parameters `config.json` as input.

To run the `single_pipe` demo, for example,

```
build/demos/single_pipe/single_pipe demos/single_pipe/config.json
```

This runs the transient simulation and for saves the state at each time instance into a HDF5 file.
The demos also provide a `plot` script to plot the pressure/momentum values at nodes as `.pdf`.

Running the demos results in state files at different time instances saved in a HDF5 file.
A `plot` script is provided to plot the results at the pipe endpoints.
For more details, refer to the local READMEs within.

A convenience script [`RUNME.sh`](RUNME.sh) is provided to run all the demos and plot results.
