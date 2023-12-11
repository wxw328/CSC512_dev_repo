#include <stdio.h>
#include <stdlib.h>

int fun(int a, int b) {
    return a + b;
}

int main(){
	int idd;
	int n = 0;
	scanf("%d, %d", &idd, &n);
    int a = fun(idd, n);
	int s = 0;
	for (int i=0;i<n;i++){
		s += rand();
	}
    char * str1[1000];
    FILE * fp = fopen ("file.txt", "r");
    char c;
    int len=0;
    while (1){
        c=getc(fp);
        if (c==EOF) break;
        str1[len++] = c;
        if (len>=1000) break; 
    }
    printf("%s\n", str1);
    fclose(fp);
}
