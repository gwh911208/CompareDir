compare:compare.c mymd5.c
	gcc -Wall -g -o compare compare.c mymd5.c
PHONY:clean
clean:
	rm compare
