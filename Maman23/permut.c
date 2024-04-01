#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "permut.h"
int atLeastOne = 0;

int main(int argc, char *argv[]) {
	checkArgs(argc-1, argv);
	contains(argv[1], argv[2]);
	if(!atLeastOne){
		printf("no permutation found\n");
	}
    return 0;
}
void checkArgs(int argc, char *argv[]){
	if(argc < 2){
		printf("too little argumetns for this program\n");
		exit(-1);
	}else if(argc > 2){
		printf("too many arguments for this program\n");
		exit(-1);
	}
}

void contains(char fileName[], char word[]){
	FILE *file = fopen(fileName, "r");
 	int ind = 0, exists = 1;
	int ch = 0;
	struct node *curr = (struct node *)malloc(sizeof(struct node)), *head = (struct node *)malloc(sizeof(struct node)), *temp = (struct node *)malloc(sizeof(struct node));;

    if (file == NULL) {
        printf("file can't be opened \n");
		exit(-1);
    }
    ch = getc(file);
    head = (struct node *)malloc(sizeof(struct node));
    head->data = ch;
    head->next = NULL;
    while(ch!=EOF){
	    /*if node contains every letter*/
        checkPerm(head, word);
        exists = 0;
        for(ind = 0; ind < strlen(word); ind++){
            if(ch == word[ind]){
                exists = 1;
                break;
            }
        }
        if(!exists){
            head->data = EOF;
        }
        else{
        	if(head->data==EOF){
        		head->data = ch;
        		head->next = NULL;
        	}
		    else{
				/*add the node*/
				temp = (struct node *)malloc(sizeof(struct node));
				curr = head;
				while(curr->next!= NULL){
					if(curr->data == ch){
				        head = curr->next;
				    }
					curr = curr->next;
				}
				curr->next = temp;
				temp->data = ch;
				temp->next = NULL;
		    }
        }
    	ch = getc(file);
    }
   	
    checkPerm(head, word);
	fclose(file);
}
/*if node contains every letter*/
void checkPerm(struct node *head, char word[]){
    int ind, exists;
    struct node *curr;
    if(head->data==EOF){
		return;
	}
    for(ind = 0; ind < strlen(word); ind++){
        exists = 0;
        for(curr = head; curr != NULL; curr = curr->next){
            if(curr->data == word[ind]){
                exists = 1;
            }
        }
        if(!exists){
            return;
        }
    }
	atLeastOne=1;
    for(curr = head; curr != NULL; curr = curr->next){
        for(ind = 0; ind < strlen(word); ind++){
        	if(curr->data == word[ind]){
                printf("%c", word[ind]);
            }
    	}
    }
	printf("\n");
    return;
}
