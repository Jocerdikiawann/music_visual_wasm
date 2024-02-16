#!/bin/sh

set -xe

emcc -o game.html main.c -Os -Wall $HOME/Documents/raylib/src/libraylib.a -I. -I$HOME/Documents/raylib/src/ -L. -L$HOME/Documents/raylib/src/ -s USE_GLFW=3 -s ASYNCIFY --shell-file $HOME/Documents/raylib/src/shell.html -DPLATFORM_WEB
