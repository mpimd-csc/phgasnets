## Chaczykowski (2009) single pipe testcase

- Pipe is 363 km-long and 1.422 m-diameter
- Scenario considered is over a 24-hr period.
- The pipe does not have horizontal profile and as such hx = 0.
- The temperature of the fluid is assumed a constant 3.1 deg C and the specific gas constant is 530 J/kg-mol.
- The supply pressure of the pipe was maintained at 84 bar at the inlet node
- The demand massflux fluctuates sharply at the outlet every 6-hr time interval,

| Time (h) | Massflux (kg/m^2-s) |
|----------|---------------|
| 0-6      | 463.33        |
| 6-12     | 540.55        |
| 12-18    | 386.11        |
| 18-24    | 463.33        |

### Run demo

Build the `single_pipe` executable following instructions in the project directory.

The program additionally needs a JSON configuration file as an argument to run.

```bash
${BUILD_DIR}/demos/single_pipe/single_pipe config.json
```
Set the environment variable or substitute `BUILD_DIR` to the build path.

The results will be written to an HDF5 file with a filename specified within the `config.json` file.

### Plot Results

Finally, the `plot` executable file can parse the config and generated results file to write a PDF plot with the same name.

```bash
./plot --config config.json
```
