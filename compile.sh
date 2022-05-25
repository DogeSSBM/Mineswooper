#!/bin/sh
clear
libraries="-lSDL2 -lSDL2_ttf -lSDL2_gfx"
flags="-Wall -Wno-missing-braces -lm -O3 -Winline"
gcc main.c $libraries $flags -o main.out
