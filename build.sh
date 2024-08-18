#!/bin/sh
mkdir -p Build
gcc -O2 -o Build/image_tool Source/main.c -L/usr/local/lib -lraylib -lGL -lm -lpthread -ldl -g
