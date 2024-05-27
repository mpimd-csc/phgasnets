
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
  - [Build and run in containers](#docker-container): Recommended for beginners to reproduce results
  - [VSCode Development containers](#vscode-development-container): Recommended for developers to enhance source code
  - [Manually configure and build source code](#build): Recommended for advanced users for fine-grained customizations.

Once the library is built, you can [**run the demos**](#run-demos) provided in the `demos/` folder.

Three demos are provided:

  - `single_pipe` demo runs a transient simulation of the Yamal-Europe pipeline configuration (without a compressor).
  - `two_pipe_compressor` demo runs the Yamal-Europe pipeline configuration with a FC-AV compressor (CR=1.2) placed midway.
  - `four_compressor_types` demo runs the Yamal-Europe pipeline configuration with all four types of compressors placed midway.

### Docker Container

The project offers two dockerfiles,
  - [`Dockerfile.dev`](Dockerfile.dev) to set the relevant dependencies and,
  - [`Dockerfile.run`](Dockerfile.run) to build the library and run the demos,
without having to install dependencies on the host.

Additionally a [`docker-compose.yml`](docker-compose.yml) is included to simplify the build process.

Necessary tools :
  - [Docker Desktop](https://docs.docker.com/desktop/) specifically Docker Engine and Docker Compose.

Build the `phgasnets-dev` image which contains the environment,

```bash
docker build -f Dockerfile.dev --tag phgasnets-dev .
```

Build the `phgasnets-run` image which contains the source code and executables,
```bash
docker build -f Dockerfile.run --tag phgasnets-run .
```

Create a folder named `results` to store results and run the container,
```bash
mkdir results
docker run --rm -it -v ${PWD}/results:/phgasnets/run phgasnets-run
```
This should run all the demos in a disposable container and store the generated PDFs in the `results` folder.
If an alternate path is desired to store the results, this maybe specified by setting an environment variable `RESULTS_DIR` before building.

> Ensure that the directory to store the results exists before running, since this needs to be mounted as shared volume within the host and container.

> Docker Compose is just provided for ease. Follow instructions within the Dockerfiles for building and running using just Docker CLI.

### VSCode Development Container

If you intend to develop the source code without modifying anything on your host computer, you can make use of [development containers](https://containers.dev/) for setting up the requisite environment,

Necessary tools :

- [Docker Engine](https://docs.docker.com/engine/install/),
- [Visual Studio Code](https://code.visualstudio.com/) with [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) extension.

Start VS Code, run the "Dev Containers: Open Folder in Container..." command from the Command Palette (F1) or quick actions Status bar item, and select the project folder.

This should start setting up the container which can take a while (~10-15 min) and open the project within the container.

Proceed to [building `phgasnets`](#build). Once the library is built, you can run the demos.

> The container specification is provided by [`Dockerfile.dev`](Dockerfile.dev) which contains all the dependencies required for the project.
> The JSON file [`.devcontainer.json`](.devcontainer.json) specifies the image to use along with VSCode extensions available within the development container.

### Build

Building requires installing the following dependencies (if not using the devcontainer):

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
* [Python3](https://www.python.org) `>=3.2` interpreter,
* [h5py](https://www.h5py.org/) package to parse HDF5 files,
* [Matplotlib](https://matplotlib.org/) library to plot,
* [Latin Modern Math](https://www.gust.org.pl/projects/e-foundry/latin-modern) font.

All these libraries are also available through standard UNIX package managers or python package managers `pip`.
The following sequence of commands builds `phgasnets`.

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

For specific details, refer to the local READMEs within.

A convenience script [`RUNME.sh`](RUNME.sh) is provided to run all the demos and plot results.
