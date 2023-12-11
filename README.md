# CSC512_dev_repo
This is the development repository for NCSU CSC 512 course project

These are instructions about how to build up this program:

1. Build the LLVM pass

```
mkdir build
cd build
cmake ..
make
cd ..
```

2. Compile external functions and example functions

```
clang -c logfunction.c
clang -fpass-plugin=build/skeleton/SkeletonPass.so -g -c example.c
```

3. Link them together

```
clang -o example logfunction.o example.o
```

4. run example

```
./example
```

##### 

To compile example3, following these commands:

```
clang -fpass-plugin=build/skeleton/SkeletonPass.so -g -c example.c
clang -c logfunction.c
clang -o example logfunction.o example.o
```

To run example3, you can use commands like:

```
./example3 3 reference.dat 0 0
```
For the discription of input content, read this link:

[https://www.spec.org/cpu2017/Docs/benchmarks/519.lbm_r.html]()
