## Chaczykowski (2009) single pipe testcase

- Pipe is 363 km-long and 1.422 m-diameter
- Scenario considered is over a 24-hr period.
- The pipe does not have horizontal profile and as such hx = 0.
- The temperature of the fluid is assumed a constant 3.1 deg C and the specific gas constant is 530 J/kg-mol.
- The supply pressure of the pipe was maintained at 84 bar at the inlet node
- The demand fluctuates sharply at the outlet every 6-hr time interval as shown,

## Run Solver

### Single Pipe Case
The executable is now built into `single_pipe` with the `build` folder, that `cmake` generated.

You need a JSON configuration file to the executable as argument to run.

```
./build/demos/single_pipe/single_pipe demos/single_pipe/config.json
```

This should write the results into a HDF5 file, with a filename specified within the `.json` file.

### Plot Results

Finally, the `plot` executable file can parse the config and results file to write a PDF plot with the same name.

```
./plot --config ./demos/single_pipe/config.json
```
