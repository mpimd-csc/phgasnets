
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

The library is structured into the following directories:

| Directory           | Information                                                                     |
|---------------------|---------------------------------------------------------------------------------|
| `src`               | `phgasnets` library source code; implementation of port-Hamiltonian formulation |
| `include/phgasnets` | `phgasnets` library headers; the public interface of the library                |
| `demos`             | the testcase demos which uses the `phgasnets` library                           |

To use the code, you must first set the environment and dependent libraries.
Instructions provided here are provided for standard UNIX distributions, but maybe easily adopted(but not tested) in other operating systems.

You may either choose to use,
  - [Build and run in containers](#docker-container): Recommended to primarily reproduce results
  - [Manually configure and build source code](#build): Recommended for fine-grained customizations and develop source.

Once the library is built, you can [**run the demos**](#run-demos) provided in the `demos/` folder.

Three demos are provided:

  - `single_pipe` demo runs a transient simulation of the Yamal-Europe pipeline configuration (without a compressor).
  - `two_pipe_compressor` demo runs the Yamal-Europe pipeline configuration with a FC-AV compressor (CR=1.2) placed midway.
  - `four_compressor_types` demo runs the Yamal-Europe pipeline configuration with a compressor placed midway modeled in four configurations.

### Docker Container

The project offers a [`Dockerfile`](Dockerfile) to automate the configuration and execution by,
setting relevant dependencies, hosting the source code, building executables and running the demos.

Necessary tools:
  - [Docker Desktop](https://docs.docker.com/desktop/) specifically Docker Engine (tested with `26.1.3`).

Build the `phgasnets` image which contains the environment, copies the source code and builds executables

```bash
docker build -f Dockerfile --tag phgasnets .
```

Create a folder named `results` to store results in the host,
```bash
mkdir results
```

Run the container by sharing the results folder within and user id information to manage the generated file permissions,
```bash
docker run --rm -u $(id -u):$(id -g) -v ${PWD}/results:/phgasnets/run phgasnets
```
This should run all the demos in a disposable container and store the generated PDFs in the `results` folder.

> Ensure that the directory to store the results exists before running, since this needs to be mounted as shared volume within the host and container. This also preserves user permissions on the files that are created within the container.

### Build

Building requires installing the following dependencies (if not using the container):

* [gcc](https://gcc.gnu.org/) (or any C++17-compliant compiler)
* [CMake](https://gitlab.kitware.com/cmake/cmake)
* [Eigen3](https://gitlab.com/libeigen/eigen) `>=3.4.0` for handling linear algebra,
* [Ceres](http://ceres-solver.org/) `>=2.0.0` for solving non-linear system of equations,
* [HDF5](https://www.hdfgroup.org/solutions/hdf5/) and [HighFive](https://bluebrain.github.io/HighFive/) `>=2.8` for writing/reading states to HDF5 format,
* [nlohmann_json](https://github.com/nlohmann/json) `>=3.10` for reading JSON configuration files.

All these dependencies **except HighFive** are available through standard UNIX package managers.
Instructions to install HighFive can be obtained [here](https://github.com/BlueBrain/HighFive/blob/v2.9.0/doc/installation.md).

> CMake offers version compatibility check for all these dependencies.

Note the locations of all the libraries in case any were not installed through standard package managers.

Additional requirements are required for `plot` scripts in the demo:
* [Python3](https://www.python.org) `>=3.4` interpreter,
* [h5py](https://www.h5py.org/) package to parse HDF5 files,
* [Matplotlib](https://matplotlib.org/) library to plot,
* [Latin Modern Math](https://www.gust.org.pl/projects/e-foundry/latin-modern) font.

All these libraries are also available either through standard UNIX package managers (for a system-wide installation) or Python package manager, `pip` (for a local installation).
The **Latin Modern Math font is optional** and not installing will result in warnings, but will not break the plot scripts.

A `requirements.txt` file is also included to install python dependencies for convenience, and maybe installed using
```bash
pip install -r requirements.txt
```

Once the dependencies are available, the following sequence of commands builds the project executables.

> Current working directory is assumed as the top-level project directory and the build files will be placed in `build` directory.

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE="Release"
```
This configures the project by finding the dependencies and generating host-specific build instructions.

To compile and create executables,
```bash
cmake --build build
```

CMake looks for the dependencies in standard UNIX paths, but if any of the dependencies are at a custom location their paths may be indicated through `-DCMAKE_PREFIX_PATH="/path/to/ceres;/path/to/highfive"`.
To compile in debug mode and utilize debuggers like [`gdb`](https://www.sourceware.org/gdb/) use `-DCMAKE_BUILD_TYPE=Debug` instead and build.

## Run Demos

The demo executables are available in the `build` directory and take configuration parameters `config.json` as input.

To run the `single_pipe` demo, for example,

```bash
build/demos/single_pipe/single_pipe demos/single_pipe/config.json
```

This runs the transient simulation and saves the state at each time instance into a HDF5 file.
A `plot` script is provided to parse the config file, read the specified HDF5 files and plot the pressure/momentum values at the pipe endpoints.

```bash
demos/single_pipe/plot -c demos/single_pipe/config.json
```

> For detailed description on each demo, refer to the READMEs within.

A convenience script [`RUNME.sh`](RUNME.sh) is provided to run all the demos and plot results.
