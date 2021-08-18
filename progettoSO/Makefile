all: utilities.o algorithm.o client.o server.o writer.o reader.o Transducers.o transducer wes.o wes \
	PFC-Disconnect-Switch.o pfc main OK

OK:
	@echo	"\nCompilazione terminata eseguire con cd bin ; ./main /posizione/G18.txt\nE' necessario spostarsi in bin per far funzionare il path location del progetto"

main:
	cc ./src/main.c -o ./bin/main

pfc: ./src/PFC-Disconnect-Switch.o ./src/reader.o ./src/utilities.o ./src/algorithm.o ./src/server.o ./src/writer.o
	cc ./src/PFC-Disconnect-Switch.o ./src/reader.o ./src/utilities.o ./src/algorithm.o ./src/server.o ./src/writer.o -lm -o ./bin/pfc

PFC-Disconnect-Switch.o: ./src/PFC-Disconnect-Switch.c ./lib/algorithm.h ./lib/socket_and_pipe.h ./lib/utilities.h
	cc -c ./src/PFC-Disconnect-Switch.c -o ./src/PFC-Disconnect-Switch.o

wes: ./src/Wes.o ./src/utilities.o ./src/writer.o
	cc ./src/Wes.o ./src/utilities.o ./src/writer.o -lm -o ./bin/wes

wes.o: ./src/Wes.c ./lib/socket_and_pipe.h ./lib/utilities.h
	cc -c ./src/Wes.c -o ./src/Wes.o

transducer: ./src/Transducers.o ./src/utilities.o ./src/client.o ./src/reader.o
	cc ./src/Transducers.o ./src/utilities.o ./src/client.o ./src/reader.o -lm -o ./bin/transducer

Transducers.o: ./src/Transducers.c ./lib/utilities.h ./lib/socket_and_pipe.h
	cc -c ./src/Transducers.c -o ./src/Transducers.o

reader.o: ./src/reader.c ./lib/main_libraries.h ./lib/utilities.h ./lib/socket_and_pipe.h
	cc -c ./src/reader.c -o ./src/reader.o

writer.o:  ./src/writer.c ./lib/main_libraries.h ./lib/utilities.h ./lib/socket_and_pipe.h
	cc -c ./src/writer.c -o ./src/writer.o

server.o: ./src/server.c ./lib/main_libraries.h ./lib/socket_and_pipe.h ./lib/utilities.h
	cc -c ./src/server.c -o ./src/server.o

client.o: ./src/client.c ./lib/main_libraries.h ./lib/utilities.h ./lib/socket_and_pipe.h
	cc -c ./src/client.c -o ./src/client.o

algorithm.o: ./src/algorithm.c ./lib/algorithm.h ./lib/socket_and_pipe.h ./lib/utilities.h
	cc -c ./src/algorithm.c -o ./src/algorithm.o

utilities.o: ./src/utilities.c ./lib/main_libraries.h ./lib/utilities.h
	cc -c ./src/utilities.c -o ./src/utilities.o

install:
	mkdir tmp
	mkdir bin
	mkdir src
	mv *.c ./src/.
	mkdir lib
	mv *.h ./lib/.
	mkdir logs

clean:
	mv ./src/*.c ./.
	mv ./lib/*.h ./.
	rm -r lib
	rm -r bin
	rm -r tmp
	rm -r src/
	rm -r logs
