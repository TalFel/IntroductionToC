all: PhaseTwo.o PhaseOne.o Util.o MacroUtil.o PhaseOneUtil.o PhasesUtil.o PreAssembler.o Main.o
	gcc -ansi -Wall -pedantic -o Compiler Main.o PhaseTwo.o PhaseOneUtil.o PhasesUtil.o PhaseOne.o Util.o MacroUtil.o PreAssembler.o -lm

Main.o: Main.c PreAssembler.h Util.h PhaseOne.h
	gcc -ansi -Wall -pedantic -c Main.c
PreAssembler.o: PreAssembler.c PreAssembler.h Util.h MacroUtil.h
	gcc -ansi -Wall -pedantic -c PreAssembler.c
MacroUtil.o: MacroUtil.c MacroUtil.h Util.h
	gcc -ansi -Wall -pedantic -c MacroUtil.c
PhaseTwo.o: PhaseTwo.c PhaseTwo.h Util.h PhasesUtil.h
	gcc -ansi -Wall -pedantic -c PhaseTwo.c
PhaseOne.o: PhaseOne.c PhaseOne.h PhaseOneUtil.h PhasesUtil.h Util.h
	gcc -ansi -Wall -pedantic -c PhaseOne.c
PhaseOneUtil.o: PhaseOneUtil.c PhaseOneUtil.h Util.h
	gcc -ansi -Wall -pedantic -c PhaseOneUtil.c
PhasesUtil.o: PhasesUtil.c PhasesUtil.h Util.h
	gcc -ansi -Wall -pedantic -c PhasesUtil.c
Util.o: Util.c Util.h
	gcc -ansi -Wall -pedantic -c Util.c

clean:
	-rm -rf *.o
	-rm -rf Compiler
run: Compiler
	./Compiler tests/TestFromMaman tests/BigFileTest tests/MacrosTest tests/ErrorsTest1 tests/ErrorsTest2
