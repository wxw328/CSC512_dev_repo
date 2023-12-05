#include <stdio.h>
#include <stdint.h>

void logfunction(void (*funcPtr)()) {
    // Cast the function pointer to uintptr_t and log it
    uintptr_t funcPtrValue = (uintptr_t)funcPtr;
    printf("Function pointer invoked: %p\n", (void*)funcPtrValue);
}

void logbranch(int branchID){
    printf("br_%d\n",branchID);
}