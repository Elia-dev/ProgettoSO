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
cc InputManager.c -o inputManager
cc P1.c -o p1
cc P2.c -o p2
cc P3.c -o p3
cc DecisionFunction.c -o decisionFunction
