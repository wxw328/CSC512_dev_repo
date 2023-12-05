# CSC512_dev_repo
This is the development repository for NCSU CSC 512 course project

To Compile example3, following these commands:
clang -fpass-plugin=build/skeleton/SkeletonPass.so -g -c example.c
clang -c logfunction.c
clang -o example logfunction.o example.o

To run example3, you can use commands like:
./example3 3 reference.dat 0 0
