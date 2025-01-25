CC = gcc
BINDIR = ./bin/
BINNAME = main
OUT = $(BINDIR)$(BINNAME)
FILES = ./main.c


build:
	make -C server build;
	gcc $(FILES) -o $(OUT);

run:
	make build;
	printf "\n\n";
	$(OUT);
