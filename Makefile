# This is a custom made make file for Chip8 Project
CC=gcc
CFLAGS=-pedantic -Wall -Wextra -I"include" -L"lib"
DEBUG=-g3 -O0
#LATEXCOMPILER=pdflatex

all: chip8.c chip8.h Main.c
	${CC} ${DEBUG} ${CFLAGS} -O2 $(shell pkg-config --cflags --libs sdl2) -o Chip8 Main.c chip8.c platform.c
#	${LATEXCOMPILER} test.tex

#docs:
#	${LATEXCOMPILER} test.tex

run: Chip8
	./Chip8
Chip8: chip8.c chip8.h Main.c
	${CC} ${CFLAGS} $(shell pkg-config --cflags --libs sdl2) -o Chip8 Main.c Chip8.c platform.c

debug: chip8.c chip8.h Main.c
	${CC} ${DEBUG} ${CFLAGS} $(shell pkg-config --cflags --libs sdl2) -o Chip8 Main.c chip8.c platform.c
	gdb --args ./Chip8 10 1 BC_test.ch8

compile: chip8.c chip8.h Main.c platform.c
