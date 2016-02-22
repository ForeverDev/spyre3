CC = gcc
CF = -std=c11
LIBS = -llua -ldl -lm
DEPS = build/main.o build/slex-byte.o build/scompile-byte.o build/slib.o build/sapi.o build/spyre.o 
BUILD = build
VPATH = src

build/spy3: $(DEPS)
	sudo $(CC) $(CF) $(DEPS) -o spy3
	sudo mv spy3 /usr/local/bin

build/main.o: src/main.c
	$(CC) $(CF) -c src/main.c -o build/main.o

build/slex-byte.o: src/interpreter/slex-byte.c
	$(CC) $(CF) -c src/interpreter/slex-byte.c -o build/slex-byte.o

build/scompile-byte.o: src/interpreter/scompile-byte.c
	$(CC) $(CF) -c src/interpreter/scompile-byte.c -o build/scompile-byte.o

build/slib.o: src/interpreter/slib.c
	$(CC) $(CF) -c src/interpreter/slib.c -o build/slib.o

build/sapi.o: src/interpreter/sapi.c
	$(CC) $(CF) -c src/interpreter/sapi.c -o build/sapi.o
	
build/spyre.o: src/interpreter/spyre.c
	$(CC) $(CF) -c src/interpreter/spyre.c -o build/spyre.o
