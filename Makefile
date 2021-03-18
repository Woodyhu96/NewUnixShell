mysh: sh.o get_path.o helperfunc.o main.c
	gcc -g main.c sh.o get_path.o helperfunc.o -o sh -lpthread


helperfunc.o: helperfunc.c helperfunc.h
	gcc -g -c helperfunc.c

sh.o: sh.c sh.h
	gcc -g -c sh.c

get_path.o: get_path.c get_path.h
	gcc -g -c get_path.c

clean:
	rm -rf sh.o get_path.o helperfunc.o sh
