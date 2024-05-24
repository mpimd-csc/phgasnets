## Two Pipes with a compressor: A modified Chaczykowski (2009) testcase

- Two Pipes are 181.5 km-long and 1.422 m-diameter
- A compressor is placed between them with a fixed compression ratio 1.2.
- Scenario considered is over a 24-hr period.
- The pipe does not have a horizontal profile and as such hx = 0.
- The temperature of the fluid at the inlet is assumed a constant 3.1 deg C and the specific gas constant is 530 J/kg-mol.
- The supply pressure of the pipe was maintained at 84 bar at the inlet node.
- The demand massflux fluctuates sharply at the outlet every 6-hr time interval,

| Time (h) | Massflux (kg/m^2-s) |
|----------|---------------|
| 0-6      | 463.33        |
| 6-12     | 540.55        |
| 12-18    | 386.11        |
| 18-24    | 463.33        |

### Run demo

Build the `two_pipe_compressor` executable following instructions in the project directory.

The program additionally needs a JSON configuration file as an argument to run.

```bash
${BUILD_DIR}/demos/two_pipe_compressor/two_pipe_compressor demos/two_pipe_compressor/config.json
```
Set the environment variable or substitute `BUILD_DIR` to the build path.

The results will be written to an HDF5 file with a filename specified within the `config.json` file.

### Plot Results

Finally, the `plot` executable file can parse the config and generated results file to write a PDF plot with the same name.

```bash
./plot --config ./demos/two_pipe_compressor/config.json -p 0
./plot --config ./demos/two_pipe_compressor/config.json -p 1
```
The argument `-p` with an integer value refers to the first and second pipes respectively.
