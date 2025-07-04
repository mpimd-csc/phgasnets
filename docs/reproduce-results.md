# Reproduce Results

You may either choose to use,
  - {ref}`Build and run in containers <run-using-docker-container>`: Recommended to primarily reproduce results
  - Build and run natively (using {ref}`spack <run-using-spack>` or {ref}`vcpkg <run-using-vcpkg>`): Recommended to reproduce results natively if containers are not an option.
  - {ref}`Manually configure and build source code <compile-and-build>`: Recommended for fine-grained customizations and develop source.

Once the library is built, you can {ref}`run the demos <run-demos>` provided in the `demos/` folder.

> For developers, a {ref}`development container <development-container-for-developers>` is also available to ease the workflow.

The following demos are provided within [`demos/`](demos/):

  - [`single_pipe`](demos/single_pipe/) runs a transient simulation of the Yamal-Europe pipeline configuration (without a compressor).
  - [`four_compressor_types`](demos/four_compressor_types/) runs the Yamal-Europe pipeline configuration with a compressor placed midway modeled in four configurations.

```{toctree}
:maxdepth: 1
:hidden:

docker
spack
vcpkg
compile
```
