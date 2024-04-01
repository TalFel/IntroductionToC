#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include "set.h"

uint8_t* getSet(int index)
{
   return &(sets[index].SET[0]);
}


void reset_set(set SET){
    memset(SET, 0, sizeof(set));
}

void read_set(set s, int numbersToAdd[], int size){
    reset_set(s);
    while(size > 0){
        markBit(s, numbersToAdd[size - 1]);
        size--;
    }
}

void print_set(set SET){
    int empty = 1;
    int counter = 0;
    int enterCounter = 0;
    for(; counter < numBits; counter++){
        if (isBitSelected(&SET[0],counter)){
           if(!empty){
              printf(", ");
           }
           empty = 0;
           enterCounter++;
           printf("%d", counter);
        }
        if(enterCounter%16 == 0 && enterCounter > 0){
           printf("\n");
        }
    }
    if(empty) printf("The set is empty");
    printf("\n");
}
void clone_set(set SET1, set SET2){
     memcpy(SET1, SET2, bitmapSize);
}
void union_set(set SET1, set SET2, set SET3){
   int counter = 0;
    reset_set(TEMPSET);
    for(; counter < numBits; counter++){
        if(isBitSelected(SET1, counter) || isBitSelected(SET2, counter)){
           markBit(TEMPSET, counter);
        }
    }
    clone_set(SET3, TEMPSET);
}
void intersect_set(set SET1, set SET2, set SET3){
    int counter = 0;
    reset_set(TEMPSET);
    for(; counter < numBits; counter++){
        if(isBitSelected(SET1, counter) && isBitSelected(SET2, counter)){
           markBit(TEMPSET, counter);
        }
    }
    
    clone_set(SET3, TEMPSET);
}
void sub_set(set SET1, set SET2, set SET3){
    int counter = 0;
    reset_set(TEMPSET);
    for(; counter < numBits; counter++){
        if(isBitSelected(SET1, counter) && !isBitSelected(SET2, counter)){
           markBit(TEMPSET, counter);
        }
    }
    clone_set(SET3, TEMPSET);

}
void symdiff_set(set SET1, set SET2, set SET3){
    int counter = 0;
    reset_set(TEMPSET);
    for(; counter < numBits; counter++){
        if(!(isBitSelected(SET1, counter) && isBitSelected(SET2, counter)) && (isBitSelected(SET1, counter) || isBitSelected(SET2, counter))){
           markBit(TEMPSET, counter);
        }
    }
    clone_set(SET3, TEMPSET);
}
void stop(){
    printf("Program stopped.\n");
    exit(0);
}


