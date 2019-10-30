.DEFAULT_GOAL = all

clean:
	@rm tp1.o

all:
	@gcc-9 -O2 -o tp1.o tp1.c

run: clean all
	@./tp1.o

