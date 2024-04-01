#include "InputManager.h"
#include <stdio.h>
int getActualSize(char str[]){
    int i = 511;
    while(str[i] == NULL && i >= 0){
        i--;
    }
    return i + 1;
}
int containsOnlyNumbers(char str[]){
    int i = getActualSize(str) - 1;
    if(i == -1) return 0;
    for(; i >= 0; i--){
        if(str[i] < '0' || str[i] > '9') return 0;
    }
    return 1;
}
void getStrInput(char str[], int maxLen){
    char chr = NULL;
    int ind = 0;
	chr = getchar();
    while(chr != EOF && ind < 512 && ind < maxLen){        
        str[ind] = chr;
        ind++;
		chr = getchar();
    }
    while(ind<512){
        str[ind] = NULL;
        ind++;
    }
}

void getNumberInput(char str[]){
    char chr = getchar();
    int ind = 0;
    while(chr != EOF && ind < 512 && chr!='\n'){
        str[ind] = chr;
        ind++;
        chr = getchar();
    }
    while(ind<512){
        str[ind] = NULL;
        ind++;
    }
}

int checkInput(int strlength, int ind1, int ind2, int len){
    if(len > 512){
        printf("illegal length to compare, must be under 512\n");
        return 0;
    }
    if(len == 0) return 1;
    if(strlength == 0){
        printf("\nilligal string, length must be at least 1 character long\n");
        return 0;
    }
    if(ind1 > strlength || ind2 > strlength){
        printf("\nan index is out of bounds\n");
        return 0;
    }
    if(len + ind1 > strlength || len + ind2 > strlength){
        printf("\nthe strings to compare are out of bounds\n");
        return 0;
    }
    /*will not actually get here, the indexes from before must be positive*/
    if(ind1 < 0 || ind2 < 0){
        printf("illigal index, must be positive\n");
        return 0;
    }
    return 1;
}
