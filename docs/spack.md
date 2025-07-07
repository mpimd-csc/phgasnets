(run-using-spack)=
# Run using spack

[Spack](https://spack.io/) is a package management tool which fetches, builds and installs multiple versions and configurations of the dependencies that `phgasnets` need in isolated environments.

A `spack.yaml` configuration file is included with the project containing the dependencies required.

Prerequisites:
* [**Git**](https://git-scm.com/) for cloning repositories

1. Clone the `phgasnets` repository and [`spack`](https://github.com/spack/spack) repository within.

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
cmake --build build --parallel
```

Since you are in the spack environment, CMake should find all the right dependencies.

4.  Run the demos

```bash
./RUNME.sh
```


:::{tip}
You can deactivate the spack environment using the following command:

```bash
eval "$(./.spack/bin/spack env deactivate --sh)"
```

:::
