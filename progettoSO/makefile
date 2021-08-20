all: avvia ConstHeader.o InputManager.o inputManager P1.o p1 P2.o p2 P3.o p3 FailureManager.o failureManager DecisionFunction.o decisionFunction Watchdog.o watchdog

watchdog: ./SRC/Watchdog.o ./SRC/ConstHeader.o
		cc ./SRC/Watchdog.o ./SRC/ConstHeader.o -o watchdog

Watchdog.o: ./SRC/Watchdog.c ./SRC/ConstHeader.h
		cc -c ./SRC/Watchdog.c

decisionFunction: ./SRC/DecisionFunction.o ./SRC/ConstHeader.o
			cc ./SRC/DecisionFunction.o ./SRC/ConstHeader.o -o decisionFunction

DecisionFunction.o: ./SRC/DecisionFunction.c ./SRC/ConstHeader.h
			cc -c ./SRC/DecisionFunction.c

failureManager: ./SRC/FailureManager.o ./SRC/ConstHeader.o
		cc ./SRC/FailureManager.o ./SRC/ConstHeader.o -o failureManager

FailureManager.o: ./SRC/FailureManager.c ./SRC/ConstHeader.h
			cc -c ./SRC/FailureManager.c

p3: ./SRC/P3.o ./SRC/ConstHeader.o
	cc ./SRC/P3.o ./SRC/ConstHeader.o -o p3

P3.o: ./SRC/P3.c ./SRC/ConstHeader.h
	cc -c ./SRC/P3.c

p2: ./SRC/P2.o ./SRC/ConstHeader.o
	cc ./SRC/P2.o ./SRC/ConstHeader.o -o p2

P2.o: ./SRC/P2.c ./SRC/ConstHeader.h
	cc -c ./SRC/P2.c

p1: ./SRC/P1.o ./SRC/ConstHeader.o
	cc ./SRC/P1.o ./SRC/ConstHeader.o -o p1

P1.o: ./SRC/P1.c ./SRC/ConstHeader.h
	cc -c ./SRC/P1.c

inputManager: ./SRC/InputManager.o ./SRC/ConstHeader.o
		cc ./SRC/InputManager.o ./SRC/ConstHeader.o -o inputManager

InputManager.o: ./SRC/InputManager.c ./SRC/ConstHeader.h
		cc -c ./SRC/InputManager.c
		
#avvia: ./SRC/Avvia.c
	#cc ./SRC/Avvia.c -o avvia

ConstHeader.o: ./SRC/ConstHeader.c ./SRC/ConstHeader.h
		cc -c ./SRC/ConstHeader.c

install:
	mkdir ./LOG
	mkdir ./LIB
	mkdir ./BIN
	mkdir ./OBJ
	mv ./SRC/*.h ./LIB
	mv *.o ./OBJ
	@# cerca nella cartella corrente (no sottocartelle) dei file che non hanno estensione e li muove in OBJ
	find . -maxdepth 1 -type f ! -name "*.*" -exec mv -t ./BIN/ {} \+
	mv ./BIN/makefile ./
	mv ./BIN/filePid ./LOG
	rm ./SRC/*.o 

clean:
	rm -r LOG
	rm -r BIN
	rm -r OBJ
	rm -r LIB

