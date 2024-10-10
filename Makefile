all:
	gcc -Lsrc/Include -Lsrc/lib -o chip8 chip8.c -lmingw32 -lSDL2main -lSDL2
