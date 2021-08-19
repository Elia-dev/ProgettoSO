#!/bin/bash
rm -r LOG
rm -r BIN
mkdir LOG
mkdir BIN

cd SRC	
cc Avvia.c -o ../BIN/avvia
cd ..
./BIN/avvia $1 $2
