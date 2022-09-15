#!/bin/sh
clear
libraries="-lm -lSDL2 -lSDL2_ttf -lSDL2_gfx -lSDL2_image"
flags="-Wall -Wextra -Wpedantic -Og -g3"
gcc main.c -std=c11 $flags $libraries -o main.out
