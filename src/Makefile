SOURCES = main.c

start:	main.c
	gcc -std=c99 -Wall -o shell $(SOURCES) -lpthread

run: shell
	./shell

clean:
	rm -rf shell