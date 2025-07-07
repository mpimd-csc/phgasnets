
            _                                      _
           | |                                    | |
     _ __  | |__    __ _   __ _  ___  _ __    ___ | |_  ___
    | '_ \ | '_ \  / _` | / _` |/ __|| '_ \  / _ \| __|/ __|
    | |_) || | | || (_| || (_| |\__ \| | | ||  __/| |_ \__ \
    | .__/ |_| |_| \__, | \__,_||___/|_| |_| \___| \__||___/
    | |             __/ |
    |_|            |___/

phgasnets
=========

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.11387852.svg)](https://doi.org/10.5281/zenodo.11387852)

Code supplement for "Modelling Gas Networks with Compressors: A Port-Hamiltonian Approach"

A speedy C++ implementation of port-Hamiltonian model for Gas Network with Compressors.

Details:
  - Isothermal Euler equation model for pipes.
  - Emphasis on including four different compressor models.
  - Space discretization with second-order central finite differences.
  - Time discretization with implicit midpoint method.
  - Jacobian computation through automatic differentiation.
  - Nonlinear solve using Levenberg–Marquardt algorithm.

Authors:
  - [Ashwin S. Nayak](https://orcid.org/0000-0002-9855-2377)
  - [Thomas Bendokat](https://orcid.org/0000-0002-0671-6291)

Affiliation:
  - [Max Planck Institute for Dynamics of Complex Technical Systems](https://www.mpi-magdeburg.mpg.de/), Magdeburg, Germany.

Repository:
  - https://gitlab.mpi-magdeburg.mpg.de/anayak/phgasnets-cpp

License:
  - GNU GPL v3, see [`LICENSE.md`](LICENSE.md).

Documentation: [![Documentation Status](https://readthedocs.org/projects/phgasnets/badge/?version=latest)](https://phgasnets.readthedocs.io/en/latest/?badge=latest)

<!-- Table of Contents -->
<details open>
<summary>Table of Contents</summary>
<ul>
  <li><a href="#getting-started">Getting Started</a></li>
  <li><a href="#run-using-docker-container">Run using docker container</a></li>
  <li><a href="#run-using-spack">Run using spack</a></li>
  <li><a href="#run-using-vcpkg">Run using vcpkg</a></li>
  <li><a href="#compile-and-build">Compile and build manually</a>
    <ul>
      <li><a href="#dependencies-if-not-using-the-container">Dependencies (if not using the container)</a></li>
      <li><a href="#configure-and-compile">Compile</a></li>
      <li><a href="#run-demos">Run demos</a></li>
    </ul>
  </li>
  <li><a href="#development-container-for-developers">Development container (for developers)</a></li>
</ul>
</details>

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
  - [Build and run in containers](#run-using-docker-container): Recommended to primarily reproduce results
  - Build and run natively (using [spack](#run-using-spack) or [vcpkg](#run-using-vcpkg)): Recommended to reproduce results natively if containers are not an option.
  - [Manually configure and build source code](#compile-and-build): Recommended for fine-grained customizations and develop source.

Once the library is built, you can [**run the demos**](#run-demos) provided in the `demos/` folder.

> For developers, a [development container](#development-container-for-developers) is also available to ease the workflow.

The following demos are provided within [`demos/`](demos/):

  - [`single_pipe`](demos/single_pipe/) runs a transient simulation of the Yamal-Europe pipeline configuration (without a compressor).
  - [`four_compressor_types`](demos/four_compressor_types/) runs the Yamal-Europe pipeline configuration with a compressor placed midway modeled in four configurations.

## Run using docker container

The project offers a [`Dockerfile`](Dockerfile) to automate the configuration and execution by, setting relevant dependencies, hosting the source code, building executables and running the demos.

Necessary tools:
  - [Docker Desktop](https://docs.docker.com/desktop/) specifically Docker Engine.

The compiled image from `Dockerfile` is already shared on [docker hub](https://hub.docker.com/r/ashwinsnayak/phgasnets). Pull the image which contains the environment, includes the source code and builds executables,

```bash
docker pull ashwinsnayak/phgasnets:1.0.0
```

Create a folder named `results` to store results in the host and run the container by sharing the results folder within and user id information to manage the generated file permissions,
```bash
mkdir results
docker run --rm -u $(id -u):$(id -g) -v ${PWD}/results:/phgasnets/run ashwinsnayak/phgasnets:1.0.0
```
This should run all the demos in a disposable container and store the generated PDFs in the `results` folder.

> Ensure that the directory to store the results exists before running, since this needs to be mounted as shared volume within the host and container. This also preserves user permissions on the files that are created within the container.

# Run using spack

[Spack](https://spack.io/) is a package management tool which fetches, builds and installs multiple versions and configurations of the dependencies that `phgasnets` need in isolated environments.

A `spack.yaml` configuration file is included with the project containing the dependencies required.

Prerequisites:
* [git](https://git-scm.com/) for cloning repositories

1. Clone this repository and [spack](https://github.com/spack/spack) repository within.

```bash
git clone https://github.com/mpimd-csc/phgasnets.git
cd phgasnets

git clone --depth 1 https://github.com/spack/spack.git .spack
```

2. Create, activate spack environment from within the project directory,

```bash
.spack/bin/spack env create phgasnets
eval "$(./.spack/bin/spack env activate --sh -p .)"
.spack/bin/spack install
```

This command should technically parse `spack.yaml` and install the C++ and Python dependencies required.

3. Build `phgasnets`,

```bash
cmake -B build -S .
cmake --build build
```
Since you are in the spack environment, CMake should find all the right dependencies.

4.  Run the demos
```bash
./RUNME.sh
```

## Run using vcpkg

The project offers a `vcpkg.json` file to fetch and build all dependencies which minimizes efforts in their installation. Unfortunately, python dependencies need to be installed manually.

Prerequisites:
* [gcc](https://gcc.gnu.org/) (or any C++17-compliant compiler)
* [CMake](https://gitlab.kitware.com/cmake/cmake) `>=3.9` for building
* [git](https://git-scm.com/) for cloning the repository
* [Python3](https://www.python.org) `>=3.9` interpreter for plots. See [uv](https://github.com/astral-sh/uv) or [pyenv](https://github.com/pyenv/pyenv) for managing Python versions.

1. Clone this repository and [vcpkg](https://github.com/microsoft/vcpkg) repository within
```bash
git clone https://github.com/mpimd-csc/phgasnets.git
cd phgasnets
git clone --depth 1 https://github.com/microsoft/vcpkg.git .vcpkg
```
2. Build `phgasnets` using `vcpkg` to handle dependencies,
```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=./.vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
```
3. Install Python dependencies for plotting scripts
```bash
python3 -m venv .venv
source .venv/bin/activate
python3 -m pip install -r requirements.txt
```
4. Run the demos
```bash
./RUNME.sh
```
This should produce all the results in the `results` folder.

## Compile and Build

### Dependencies (if not using the container)

Building requires installing the following dependencies:

* [gcc](https://gcc.gnu.org/) (or any C++17-compliant compiler)
* [CMake](https://gitlab.kitware.com/cmake/cmake) `>=3.9` for building,
* [Eigen3](https://gitlab.com/libeigen/eigen) `>=3.4.0` for handling linear algebra,
* [Ceres](http://ceres-solver.org/) `>=2.0.0` for solving non-linear system of equations,
* [HDF5](https://www.hdfgroup.org/solutions/hdf5/) library with [HighFive](https://bluebrain.github.io/HighFive/) `>=2.8` interface for writing/reading states to HDF5 format,
* [nlohmann_json](https://github.com/nlohmann/json) `>=3.10` for reading JSON configuration files.

All these dependencies **except HighFive** are available through standard UNIX package managers.
Instructions to install HighFive can be obtained [in their repository](https://github.com/BlueBrain/HighFive/blob/v2.9.0/doc/installation.md).

> CMake offers version compatibility check for all these dependencies.

Note the locations of all the libraries in case any were not installed through standard package managers.

Additional requirements are required for `plot` scripts in the demo,
* [Python3](https://www.python.org) `>=3.9` interpreter,
* [numpy](https://numpy.org) package for handling arrays,
* [matplotlib](https://matplotlib.org/) plotting package,

All these libraries are also available either through standard UNIX package managers (for a system-wide installation) or Python package manager, `pip` (for a local installation).

A [`requirements.txt`](requirements.txt) file is also included to install python dependencies for convenience, and maybe installed using
```bash
python3 -m venv .venv
. .venv/bin/activate
python3 -m pip install -r requirements.txt
```

### Configure and Compile

Once the dependencies are available, the following sequence of commands builds the project executables.

> Current working directory is assumed as the top-level project directory and the build files will be placed in `build` directory.

```bash
cmake -B build -S .
```
This configures the project by finding the dependencies and generating host-specific build instructions.

> CMake looks for the dependencies in standard UNIX paths, but if any of the dependencies are at a custom location their paths may be indicated by e.g.,`-DCMAKE_PREFIX_PATH="/path/to/ceres;/path/to/highfive"`.

> Automatic differentiation is enabled by default for Jacobian construction. If you prefer to use finite-difference based numeric differentiation, configure with `-DPHGASNETS_NUMERICDIFF=ON` flag. Read more about how automatic derivatives are computed [here](http://ceres-solver.org/automatic_derivatives.html).

> To compile in debug mode, you need to disable excessive compiler optimizations so as to utilize debuggers like [`gdb`](https://www.sourceware.org/gdb/). Configure using `-DCMAKE_BUILD_TYPE="Debug"` flag.

To compile and create executables,
```bash
cmake --build build
```

### Run Demos

The demo executables are available in the `build` directory and take configuration parameters `config.json` as input.

A convenience script [`RUNME.sh`](RUNME.sh) is provided to run all the demos and plot results,

```bash
./RUNME.sh
```

> For detailed description on each demo, refer to the READMEs within.

## Development Container (for developers)

If you intend to develop the source code without modifying/installing any dependencies on your host computer, you can make use of [development containers](https://containers.dev/) for setting up the requisite environment.

A specification file [`.devcontainer.json`](.devcontainer.json) is provided for building the devcontainer and can be utilized by supporting editors.

Necessary tools :
- [Docker Engine](https://docs.docker.com/engine/install/) (tested with `26.1.3`),
- [Visual Studio Code](https://code.visualstudio.com/) with [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) extension (support since 2019. Tested with version `1.87.2`).

Start VS Code, run the "Dev Containers: Open Folder in Container..." command from the Command Palette (F1), and select the project folder.
While done for the first time, this should start building the container and can take a while (~5-10 min) and subsequently open the project within the container.
The build is cached for subsequent runs and should be fairly quick thereon.

Proceed to [compiling `phgasnets`](#compile-and-build) in the terminal and once the library is built, you can run the demos.
