#include<stdio.h>

void fun(int a){
    printf("%d\n",a);
}

int main(){
    void (*fun_ptr)(int) = &fun;
    (*fun_ptr)(10);

    int d = 0;
    for (int c=0; c<3; c++){
         d = d + 1;
    }

    return d;
}

