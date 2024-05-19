
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
  - [VSCode Development containers](#development-container)
  - [Manually build and use docker containers]()
  - Manually configure and build source code

Once the library is built, you can **run the demos** provided in the `demos/` folder.

Three demos are provided:

  - `single_pipe` demo runs a transient simulation of the Yamal-Europe pipeline configuration (without a compressor).
  - `two_pipe_compressor` demo runs the Yamal-Europe pipeline configuration with a FC-AV compressor (CR=1.2) placed midway.
  - `four_compressor_types` demo runs the Yamal-Europe pipeline configuration with all four types of compressors placed midway.


### VSCode Development Container

You can make use of [development containers](https://containers.dev/) to set up the environment, run the demos and also develop the codes in this repository.

Necessary tools :

- [Docker](https://docs.docker.com/engine/install/),
- [Visual Studio Code](https://code.visualstudio.com/) with [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) extension.

Start VS Code, run the "Dev Containers: Open Folder in Container..." command from the Command Palette (F1) or quick actions Status bar item, and select the project folder.
This should start setting up the container which can take a while (~10-15 min) and open the project within the container.
Proceed to [building phgasnets](#build).

> The container specification is provided by `Dockerfile.dev` which contains all the dependencies required for the project.
> The JSON file [`.devcontainer.json`](.devcontainer.json) specifies the image to use along with VSCode extensions available within the development container.

### Docker Container

The project offers two dockerfiles [`Dockerfile.dev`](Dockerfile.dev) and [`Dockerfile.run`](Dockerfile.run) with the necessary instructions to set the relevant environment and run the codes without having to install dependencies on the host.

First, build the base imag with all dependencies using,
```bash
docker build -t phgasnets-dev -f Dockerfile.dev .
```

Second, build the final image which contains all the source code and executables,
```bash
docker build -t phgasnets -f Dockerfile.run .
```

Create a folder named `results` to store results and run the container specifying the `results` folder,
```bash
mkdir results
docker run --rm -v ${PWD}/results:/phgasnets/run phgasnets
```
This should run all the demos and store the generated PDFs in the `results` folder.

### Build

Building requires the following dependencies (if not using the devcontainer):

* A C++17-compliant compiler e.g. [gcc](https://gcc.gnu.org/)
* [CMake](https://gitlab.kitware.com/cmake/cmake) `>= 3.9`
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

This runs the transient simulation and saves the state at each time instance into a HDF5 file.
A `plot` script is provided to parse the config file, read the specified HDF5 files and plot the pressure/momentum values at the pipe endpoints.

```
demos/single_pipe/plot -c demos/single_pipe/config.json
```

For specific details, refer to the local READMEs within.

A convenience script [`RUNME.sh`](RUNME.sh) is provided to run all the demos and plot results.
