## Two Pipes with four different compressor types: A modified Chaczykowski (2009) testcase

- Two Pipes are 181.5 km-long and 1.422 m-diameter
- A compressor is placed between them with a variable configuration.
- Scenario considered is over a 24-hr period.
- The pipe does not have a horizontal profile and as such hx = 0.
- The temperature of the fluid at the inlet is assumed a constant 3.1 deg C and the specific gas constant is 530 J/kg-mol.
- The supply pressure of the pipe was maintained at 80 bar at the inlet node.
- The demand massflux fluctuates sharply at the outlet every 6-hr time interval,

| Time (h) | Massflux (kg/m^2-s) |
|----------|---------------|
| 0-6      | 463.33        |
| 6-12     | 540.55        |
| 12-18    | 386.11        |
| 18-24    | 463.33        |

Four different compressor types are mentioned in the various `.json` files,

| Compressor Type | Framework                       | Assumption        | `CONFIG_FILE`      |
|-----------------|---------------------------------|-------------------|--------------------|
| FPAM            | Fixed Output Pressure of 84 bar | Constant Momentum | `config_fpam.json` |
| FPAV            | Fixed Output Pressure of 84 bar | Constant Velocity | `config_fpav.json` |
| FCAM            | Fixed Compression Ratio of 1.2  | Constant Momentum | `config_fcam.json` |
| FCAV            | Fixed Compression Ratio of 1.2  | Constant Velocity | `config_fcav.json` |

### Run demo

Build the `four_compressor_types` executable following instructions in the project directory.

The program additionally needs a JSON configuration file as an argument to run.

```bash
${BUILD_DIR}/demos/four_compressor_types/four_compressor_types ${CONFIG_FILE}
```

Set the environment variable or substitute `BUILD_DIR` to the build path.

Additionally, the specific `CONFIG_FILE` can be substituted from the table in the above section.

The results will be written to an HDF5 file with a filename specified within the configuration file.

### Plot Results

Finally, the `plot` executable file can parse the config and generated results file to write a PDF plot with the same name.

```bash
./plot --config ${CONFIG_FILE} -p 0
./plot --config ${CONFIG_FILE} -p 1
```

The argument `-p` with an integer value refers to the first and second pipes respectively.

An additional flag `--csv` can be used to output plot data into a `.csv` file.

For plotting a unified plot with all the compressor configurations, you can use the `plotall` script.
This requires the directory with `.csv` files generated through `plot` script for the four compressor types mentioned through `OUT_DIR`.

```bash
./plotall -O ${OUT_DIR}
```