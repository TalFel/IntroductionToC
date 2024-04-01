#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include "set.h"
#include "myset.h"

const char *commands[7] = { "read_set", "print_set", "union_set", "intersect_set", "sub_set", "symdiff_set", "stop"};
const int sizes[7] = {8, 9, 9, 13, 7, 11, 4};
void clear(char []);



void clear(char command[]){
    int i = 0;
    for(; i < 256; i++){
        command[i] = '\0';
    }
}


void act(char command[], int size){
    /*finding the command*/
    int cmdSize = 0, i = 0, commandNo = -1, valid, j;
	int currComma = 0;
    int hitLetter = 0;
    int readEnd = size;
    int lastLetter = 0;
	char curr;
	char *ptr;
	int found, k;
	int setNo, count, num, temp;
	int lastComma;
    int wasSpace;
	int *arr;
	int SET1, SET2, SET3;

    while(!isspace(command[cmdSize])) cmdSize++;
    for(; i < 7; i++){
        valid = 0;
        if(cmdSize == sizes[i]){
            valid = 1;
            j = 0;
            for(; j < cmdSize; j++){
                if(command[j] != commands[i][j]){
                    valid = 0;
                }
            }
        }
        if(valid){
            commandNo = i;
        }
    }
    /*invalid command name*/
    if(commandNo == -1){
        printf("Invalid command name.\n");
        return;
    }
    /*----------------*/
    /*stop command*/
    if(commandNo == 6){
        i = 4;
        while(i<size){
            if(!isspace(command[i])){
                printf("Spare chars at the end of the command.\n");
                return;
            }
            i++;
        }
        stop();
    }

    /*----------------*/
    /*not a space after command name*/
    if(!isspace(command[cmdSize])){
        printf("Not a white space after command name\n");
        return;
    }
    
    for(i = cmdSize; i < size; i++){
        curr = command[i];
        /*comma at the beggining*/
        if(!hitLetter && isalpha(curr)){
            hitLetter = 1;
        }
        if(!hitLetter && curr == ','){
            printf("Illigal comma between command and args.\n");
            return;
        }
        /*comma at the end*/
        if(isalpha(curr) || isdigit(curr)){
            lastLetter = 1;
        }
        else if(curr==','){
            lastLetter = 0;
        }
        /*double comma*/
        if(currComma && curr == ','){
            printf("Multiple consecutive commas.\n");
            return;
        }
        if(currComma && curr != ',' && !isspace(curr)) currComma = 0;
        if(!currComma && curr == ',') currComma = 1;
        
        /*illigal character in the string [-\=/?'"..*/
        if(!isspace(curr) && isalpha(curr) == 0 && !isdigit(curr) && curr != ','){
            if(commandNo == 0 && curr=='-'){
                readEnd = i - 1;
                if(i+1 < size){
                    if(command[i+1]!='1'){
                        printf("unidentified character.\n");
                        return;
                    }
                    j = i+2;
                    while(j < size){
                        if(!isspace(command[j])){
                            printf("Extraneous text after -1.\n");
                            return;
                        }
                        j++;
                    }
                }
                else{
                    printf("Unidentified character.\n");
                    return;
                }
            }
            else{
                printf("Unidentified character.\n");
                return;
            }
        }
    }
    /*if last non space char was comma*/
    if(!lastLetter){
        printf("Illigal comma at the end.\n");
        return;
    }
    
    setNo = 0;
    
    /*finding the sets*/
    i = cmdSize;
    while(i < size){
        if(!isspace(command[i])){
            if(isalpha(command[i])){
                if(i < size - 3){
                    count = i;
                    while(isalpha(command[count])){
                        count++;
                    }
                    if(count < size){
                        if(command[count]!=',' && !isspace(command[count])){
                            printf("Unidetified argument for this function.\n");
                            return;
                        }
                    }
                    if(count-i != 4){
                        printf("Unidetified argument for this function.\n");
                        return;
                    }
                    ptr = (char *)malloc(4 * sizeof(char));
                    j = 0;
                    found = 0;
                    for(;j < 6; j++){
                        k = 0;
                        found = 1;
                        for(; k < 4; k++){
                            if(command[i+k] != sets[j].name[k]){
                                found = 0;
                                break;
                            }
                            ptr[k] = command[i+k];
                        }
                        if(found == 1){
                            if(setNo == 0){
                                SET1 = j;
                            }
                            else if(setNo == 1){
                                SET2 = j;
                            }
                            else if(setNo == 2){
                                SET3 = j;
                            }
                            else{
                                printf("Too many arguments for this function.\n");
                                return;
                            }
                            setNo++;
                            i+=3;
                            break;
                        }
                    }
                    if((commandNo == 0 || commandNo == 1) && setNo > 1){
                        printf("Extraneous text after command.\n");
                        return;
                    }
                    if(!found){
                        if(commandNo != 0 && commandNo != 1){
                            printf("Unidetified arguments for this function.\n");
                            return;
                        }
                        else if(setNo > 0){
                            printf("Extraneous text after command.\n");
                            return;
                        }
                    }
                }
                else{
                    printf("Unidetified arguments for this function.\n");
                    return;
                }
            }
        }
        i++;
    }
    if(commandNo != 0 && commandNo != 1 && setNo < 3){
		printf("Too little arguments for this function.\n");
		return;
	}
	
    /*checking if a comma is missing*/
    i = cmdSize;
    lastComma = 1;
    wasSpace = 0;
    while(i < size){
        if(command[i]==','){
            lastComma = 1;
            wasSpace = 0;
        }
        if(isalpha(command[i]) || isdigit(command[i])){
            if(!lastComma && wasSpace){
                printf("Missing comma.\n");
                return;
            }
            lastComma = 0;
            wasSpace = 0;
        }
        if(isspace(command[i])){
            wasSpace = 1;
        }
        i++;
    }
    
    if(commandNo == 0 || commandNo == 1){ /*==read set or == print set*/
        if(setNo > 1){
            printf("Too many arguments for this function.\n");
            return;
        }
        switch(commandNo){
            case 0:
                /*check if numbers are in range and counting how much to alloc*/
                if(readEnd == size){
                    printf("command is not terminated properly.\n");
                    return;
                }
                i = cmdSize;
                count = 0;
                while(i < readEnd){
                    if(isdigit(command[i])){
                        j = i;
                        while(isdigit(command[j])){ j++; }
						
                        num = 0;
                        k = 0;
                        for(; k < j - i; k++){
                            num += pow(10, j-i-k-1) * ((int)command[i + k] - 48);
                        }
                        if(num > 127 || num < 0){
                            printf("Element is out of range.\n");
                            return;
                        }
                        count++;
                        i=j-1;
                    }
                    i++;
                }
                /*alloc an array for the numbers and fills it*/
                arr = (int *)malloc(count * sizeof(int));
                i = cmdSize;
                temp = count;
                while(i < readEnd){
                    if(isdigit(command[i])){
                        j = i;
                        while(isdigit(command[j])){ j++; }
                        num = 0;
                        k = 0;
                        for(; k < j - i; k++){
                            num += pow(10, j-i-k-1) * ((int)command[i + k] - 48);
                        }
                        temp--;
                        arr[temp] = num;
                        i = j -1;
                        
                    }
                    i++;
                }
                read_set(getSet(SET1), arr, count);
                break;
            case 1:
                print_set(getSet(SET1));
        }
    }
    else{ /*==not read set, expecting 3 sets with 2 commas between.*/
        if(setNo < 2){
            printf("Too little arguments for this function.\n");
            return;
        }
        switch(commandNo){
            case 2:
                union_set(getSet(SET1), getSet(SET2), getSet(SET3));
                break;
            case 3:
                intersect_set(getSet(SET1), getSet(SET2), getSet(SET3));
                break;
            case 4:
                sub_set(getSet(SET1), getSet(SET2), getSet(SET3));
                break;
            case 5:
                symdiff_set(getSet(SET1), getSet(SET2), getSet(SET3));
                break;
        }
    }
}
int main() {
    int ind; char in, command[256];
		
	sets[0].name = "SETA";
	sets[1].name = "SETB";
	sets[2].name = "SETC";
	sets[3].name = "SETD";
	sets[4].name = "SETE";
	sets[5].name = "SETF";
	sets[6].name = "TEMP";	
	in = getchar();
	ind = 0;
    while(in != EOF){
        command[ind] = in;
        ind++;
        if(in == '\n'){
            act(command, ind - 1);
            clear(command);
            ind = 0;
        }
        if(ind == 256){
            printf("command too long\n");
            clear(command);
            ind = 0;
        }
        in = getchar();
    }
    printf("Program abruptly stopped.\n");
	exit(-1);
    return 0;
}
