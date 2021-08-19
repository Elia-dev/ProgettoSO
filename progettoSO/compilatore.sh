#!/bin/bash
rm inputManager
rm p1
rm p2
rm p3
rm pipeP1
rm decisionFunction
rm socketP2
rm socketDF
rm fileP3
rm filePid
rm system_log
rm voted_output
rm failureManager
rm avvia
rm watchdog
cc Avvia.c -o avvia
cc InputManager.c -o inputManager
cc P1.c -o p1
cc P2.c -o p2
cc P3.c -o p3
cc DecisionFunction.c -o decisionFunction
cc FailureManager.c -o failureManager
cc Watchdog.c -o watchdog
