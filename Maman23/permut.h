#ifndef PERMUT_H
#define PERMUT_H
typedef struct node{
    char data;
    struct node *next;
}elem;

int main(int, char *[]);
void checkArgs(int, char *[]);
void contains(char [], char []);
void checkPerm(struct node *, char []);

#endif
