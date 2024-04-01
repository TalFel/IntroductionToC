all: maman22

maman22: myset.o set.o
	gcc -ansi -Wall -pedantic -o maman22 myset.o set.o -lm
myset.o: myset.c
	gcc -ansi -Wall -pedantic -c myset.c
set.o: set.c
	gcc -ansi -Wall -pedantic -c set.c
