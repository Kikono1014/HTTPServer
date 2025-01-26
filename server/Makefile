CC = gcc
BINDIR = ../bin/
BINNAME = server
OUT = $(BINDIR)$(BINNAME)
FILES = ./request/request.c ./response/response.c ./main.c

build: $(FILES)
	gcc $(FILES) -o $(OUT);

run:
	make build;
	printf "\n\n";
	$(OUT);

leaks:
	gcc $(FILES);
	valgrind --leak-check=full ./a.out;
	rm a.out