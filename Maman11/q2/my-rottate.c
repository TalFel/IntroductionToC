#include <stdio.h>
#include <string.h>

unsigned int my_rottate (unsigned int, int);
int log(int);
void print2(unsigned int);
void printMessages(unsigned int);

int main (void)
{
    printf("please enter a number and the number of shifts(with a space between): ");
    int num = 0;
    int k = 0;
    scanf("%d %d", &num, &k);
    unsigned int n = my_rottate(num, k);
    printMessages(num);
    printMessages(n);
    
    return 0;
}
void printMessages(unsigned int num){
    printf("\nbase 2-");
    print2(num);
    printf(", base 8- %o, base 10- %d, base 16- %x \n", num, num, num);
}

int log(int a){
    int counter = 0;
    while(a >= 1){
	counter++;
	a=a/2;
    }
    return counter;
}
void print2(unsigned int a){
    unsigned i;
    for (i = 1 << 31; i > 0; i = i / 2){
	if(a & i){
	    printf("1");
	}
	else{
	    printf("0");
	}
    }
}

unsigned int my_rottate (unsigned int a, int b){
    printf("%d %d", a, b);
    int len = log((int)a);
    while(b<0){
        b+=len;
    }
    for(; b > 0; b--){
        unsigned int temp = a&1;
        a>>=1;
        a = a | (temp<<(len-1));
    }
    return a;
}
