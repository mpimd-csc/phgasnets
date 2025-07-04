(run-using-vcpkg)=
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
