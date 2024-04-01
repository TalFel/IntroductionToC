all: driver

driver: driver.o my_bcmp.o InputManager.o
	gcc -ansi -Wall -pedantic -o driver driver.o my_bcmp.o InputManager.o

Main.o: driver.c
	gcc -ansi -Wall -pedantic -c driver.c

my_bcmp.o: my_bcmp.c
	gcc -ansi -Wall -pedantic -c my_bcmp.c

InputManager.o: InputManager.c
	gcc -ansi -Wall -pedantic -c InputManager.c

