# CSC512_dev_repo
This is the development repository for NCSU CSC 512 course project

To compile example3, following these commands:

```
clang -fno-discard-value-names -fpass-plugin=build/seminalInputFeatureDetection/SIFDPass.so -g -c example3.c
clang -c logfunction.c
clang -o example logfunction.o example.o -lm
```

To run example3, you can use commands like:

```
./example3 3 reference.dat 0 0
```
For the discription of input content, read this link:

[https://www.spec.org/cpu2017/Docs/benchmarks/519.lbm_r.html]()

Input arguments overview (from command line):

```
lbm <time steps> <result file> <0: nil, 1: cmp, 2: str> <0: ldc, 1: channel flow> [<obstacle file>]
Description of the arguments:

<time steps>
number of time steps that should be performed before storing the results
<result file>
name of the result file
<0: nil, 1: cmp, 2: str>
determines what should be done with the specified result file: action '0' does nothing; with action '1' the computed results are compared with the results stored in the specified file; action '2' stores the computed results (if the file already exists, it will be overwritten)
<0: ldc, 1: channel flow>
chooses among two basic simulation setups, lid-driven cavity (shear flow driven by a "sliding wall" boundary condition) and channel flow (flow driven by inflow/outflow boundary conditions)
[<obstacle file>]
optional argument that specifies the obstacle file which is loaded before the simulation is run
```

The seminal input should be time steps, i.e. the first command line input; The number of command line inputs, which changes the prgram's setup; And the result file, which creates more IOs.