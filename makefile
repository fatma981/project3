game:main.o back.o
	gcc -g main.c back.c -o game -lSDL2 -lSDL2_ttf -lSDL2_image -lm
back.o:back.c
	gcc -c back.c -g
main.o:main.c
	gcc -c main.c -g
