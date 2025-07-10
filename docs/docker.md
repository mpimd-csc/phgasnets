(run-using-docker-container)=
# Run using docker container

The project offers a `Dockerfile` to automate the configuration and execution by,
setting relevant dependencies, hosting the source code, building executables and running the demos.

Necessary tools:
  - [Docker Desktop](https://docs.docker.com/desktop/) specifically Docker Engine (tested with `26.1.3`).

The compiled image from `Dockerfile` is already shared on [DockerHub](https://hub.docker.com/r/ashwinsnayak/phgasnets). Pull the image which contains the environment, includes the source code and builds executables,

```bash
docker pull ashwinsnayak/phgasnets:1.1.0 --tag phgasnets
```

:::{note}
Alternatively, you can build the `phgasnets` image which contains the environment, copies the source code and builds executables

```bash
docker build -f Dockerfile --tag phgasnets:1.1.0 .
```
:::

Create a folder named `results` to store results in the host, and run the container by sharing the results folder within and user id information to manage the generated file permissions,
```bash
mkdir results
docker run --rm -u $(id -u):$(id -g) -v ${PWD}/results:/phgasnets/run phgasnets:1.0.0
```
This should run all the demos in a disposable container and store the generated PDFs in the `results` folder.

:::{tip}
Ensure that the directory to store the results exists before running, since this needs to be mounted as shared volume within the host and container. This also preserves user permissions on the files that are created within the container.
:::

(development-container-for-developers)=
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
