(compile-and-build)=
# Compile and Build

## Dependencies

Building requires installing the following dependencies:

* [gcc](https://gcc.gnu.org/) (or any C++17-compliant compiler)
* [CMake](https://gitlab.kitware.com/cmake/cmake) `>=3.9` for building,
* [Eigen3](https://gitlab.com/libeigen/eigen) `>=3.4.0` for handling linear algebra,
* [Ceres](http://ceres-solver.org/) `>=2.0.0` for solving non-linear system of equations,
* [HDF5](https://www.hdfgroup.org/solutions/hdf5/) library with [HighFive](https://bluebrain.github.io/HighFive/) `>=2.8` interface for writing/reading states to HDF5 format,
* [nlohmann_json](https://github.com/nlohmann/json) `>=3.10` for reading JSON configuration files.

All these dependencies **except HighFive** are available through standard UNIX package managers.
Instructions to install HighFive can be obtained [in their repository](https://github.com/BlueBrain/HighFive/blob/v2.9.0/doc/installation.md).

:::{note}
CMake offers version compatibility check for all these dependencies.
:::

The locations of all the libraries need to be noted especially where these were not installed through standard package managers.

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

## Configure and Compile

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

(run-demos)=
## Run Demos

The demo executables are available in the `build` directory and take configuration parameters `config.json` as input.

A convenience script [`RUNME.sh`](RUNME.sh) is provided to run all the demos and plot results,

```bash
./RUNME.sh
```

:::{note}
For detailed description on each demo, refer to the READMEs within.
:::
