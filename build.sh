#!/bin/bash
g++ -c *.cpp
g++ *.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
rm *.o