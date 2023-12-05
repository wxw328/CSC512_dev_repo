# CSC512_dev_repo
This is the development repository for NCSU CSC 512 course project

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
