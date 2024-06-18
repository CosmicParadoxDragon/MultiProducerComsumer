CC=gcc
CC_FLAGS=-Wall

skprog4: skprog4.c
	${CC} $(CC_FLAGS) -o skprog4 skprog4.c


run:
	./skprog4 10 1 1


