#include <stdio.h>
#include "my_bcmp.h"

int main(){
	int MAX_SIZE = 512;
    char len[MAX_SIZE];
    char ind1[MAX_SIZE];
    char ind2[MAX_SIZE];
    char str[MAX_SIZE];
    
    printf("\nPlease insert the length of bytes to compare(press enter to continue): ");
    getNumberInput(len);
    if(containsOnlyNumbers(len) == 0){
        printf("\nillegal length to compare, must be positive integer\n");
        return 0;
    }
    
    printf("\nplease insert the first index: ");
    getNumberInput(ind1);
    if(containsOnlyNumbers(ind1) == 0){
        printf("\nillegal first index, must be positive intege\n");
        return 0;
    }
    
    printf("\nplease insert the second index: ");
    getNumberInput(ind2);
    if(containsOnlyNumbers(ind2) == 0){
        printf("\nillegal second index, must be positive intege\n");
        return 0;
    }
    
    int i1 = atoi(ind1);
    int i2 = atoi(ind2);
    int length = atoi(len);
    
    int maxLen = i1 + length;
    if(i2 + length > maxLen){
        maxLen = i2 + length;
    }
    printf("\nplease insert the string: ");
    getStrInput(str, maxLen);
	printf("\n------------------\n");
	printf("the relevant string(to the maximum index we will check): %s\nthe first index: %s, the second index: %s\nthe number of bytes to compare: %s", str, ind1, ind2, len);
	printf("\n------------------\n");
    if(checkInput(getActualSize(str), i1, i2, length) == 0) return 0;
    
    if(my_bcmp2(&str[i1], &str[i2], length) == 0){
        printf("\nthe bytes are identical!\n");
    }
    else{
        printf("\nthe bytes aren't identical):\n");
    }
	return 0;
}
