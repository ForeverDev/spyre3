CC = gcc
CF = -std=c11
LIBS = -llua -ldl -lm
DEPS = main.o slex-byte.o scompile-byte.o slib.o sapi.o spyre.o 

spy3: $(DEPS)
	sudo $(CC) $(CF) $(DEPS) -o spy3
	sudo mv spy3 /usr/local/bin
	rm -Rf *.o

install: spy3
	install spy3 /usr/local/bin

%.o: %.c
	$(CC) $(CF) -c -o $@ $<

