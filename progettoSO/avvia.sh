#!/bin/bash
cc Avvia.c -o avvia
./avvia $1 $2
sleep 1.5
./inputManager
./p1
./p2
sleep 0.250
./p3
./decisionFunction
./failureManager

