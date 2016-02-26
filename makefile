CC = gcc
CF = -std=c11
LIBS = -llua -ldl -lm
DEPS = build/main.o build/assembler-lex.o build/assembler.o build/lib.o build/api.o build/spyre.o 
BUILD = build

all: build/spy3

build:
	mkdir build

build/spy3: build $(DEPS)
	sudo $(CC) $(CF) $(DEPS) $(LIBS) -o spy
	sudo mv spy /usr/local/bin
	mkdir -p /usr/local/include/spyre
	sudo cp -Rf src/compiler/tiny.spys /usr/local/include/spyre/tiny.spys
	rm -Rf build/*

build/main.o: src/main.c
	$(CC) $(CF) -c src/main.c -o build/main.o

build/assembler-lex.o: src/interpreter/assembler-lex.c
	$(CC) $(CF) -c src/interpreter/assembler-lex.c -o build/assembler-lex.o

build/assembler.o: src/interpreter/assembler.c
	$(CC) $(CF) -c src/interpreter/assembler.c -o build/assembler.o

build/lib.o: src/interpreter/lib.c
	$(CC) $(CF) -c src/interpreter/lib.c -o build/lib.o

build/api.o: src/interpreter/api.c
	$(CC) $(CF) -c src/interpreter/api.c -o build/api.o
	
build/spyre.o: src/interpreter/spyre.c
	$(CC) $(CF) -c src/interpreter/spyre.c -o build/spyre.o
