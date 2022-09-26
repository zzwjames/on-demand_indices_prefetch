main:main.o
	g++ main.o -o main
main.o:main.S
	g++ -c main.S -o main.o
main.S:main.i
	g++ -S main.i -o main.S
main.i:main.c
	g++ -E main.c -o main.i
