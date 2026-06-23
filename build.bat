@echo off
g++ main.cpp -o build/pong.exe -lraylib -lopengl32 -lgdi32 -lwinmm -I ./include -L ./lib -mwindows
call "./build/pong.exe"
