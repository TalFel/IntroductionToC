#include  <stdio.h>
#include <string.h>
void que1(char[]);
_Bool IsCapital(char);
_Bool IsLower(char);
void printMessages(char[]);

int main(void)
{
    printf("\nplease enter a string: ");
    char a[80];
    scanf("%s", a);
    printMessages(a);
    return 0;
}

void printMessages(char a[]){
    printf("\nthe string: " );
    int i;
    for(i = 0; i < strlen(a); i++){
        printf("%c", a[i]);
    }
    que1(a);
    printf("\nthe shortened string is: ");
    for(i = 0; i < strlen(a); i++){
        printf("%c", a[i]);
    }
    printf("\n");
}

void que1(char a[]){
    /*temp variable- the current length of the sequence.*/
    int len = 1;
    /*contains the last character.*/
    char last = a[0];
    /*a variable that tells us id the sequence is of capital letters or not.*/
    int cap = 0;
    /*the margin in ascii between capital and non capital letters.
    the varriable margin is 0 when the sequence is of capital letters and 32 otherwise.*/
    int margin = 32;
    if(IsCapital(a[0])){
        cap =1;
        margin = 0;
    }
    int i;

    for(i = 1; i < strlen(a); i++){
        /*checks if the current letter continues the sequence.*/
        if((int)last == (int)a[i]-1 || 
	  ((int)a[i] == (int)'A' + margin && (int)last ==(int)'Z' + margin)){
            len++;
			/*if the sequence length is equal to or larger than 3, we will put a 0 in the place of the last letter.*/
            if(((cap == 1 && IsCapital(a[i]))||(cap == 0 && IsLower(a[i])))&&(len>=3)){
                a[i-1] = '0';
            }
        }
		/*if the current letter does not continue the sequence we will restart the variables.*/
        else{
            len = 1;
            cap = 0;
            margin = 32;
            if(IsCapital(a[i])){
                cap = 1;
                margin = 0;
            }
        }
		/*saves the last letter for the next iteration.*/
        last = a[i];
    }
    /*in the second part of the program we will shorten the string.
	the writing index*/
    int w = 1;
	/*the reading index*/
    int r;
    for(r = 1; r < strlen(a); r++){
		/*if the last char we put was a letter and we are reading a 0 we will put a '-' after the letter.*/
        if((IsCapital(a[w-1]) || IsLower(a[w-1]))){
            if(a[r] == '0'){
                a[w] = '-';
                w++;
            }
        }
		/*if the current letter we are reading is a letter we will push it to the end of the shortend string(to the writing index).*/
        if(IsCapital(a[r]) || IsLower(a[r])){
            char temp = a[r];
            a[r] = '0';
            a[w] = temp;
            w++;
        }
    }
	/*to finish - we'll set the rest of the string to be null*/
    int p;
    for(p = w; p < strlen(a); w++){ 
        a[p] = NULL;
    }
}
/*checks if the char is a capital letter*/
_Bool IsCapital(char c){
    return (int)c <= 90 && (int)c >= 65;
}
/*checks if the char is a lowercase letter*/
_Bool IsLower(char c){
    return (int)c <= 122 && (int)c >= 97;
}


