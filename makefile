spy3: 
	gcc main.c spyre.c sapi.c slib.c slex-byte.c -o spy3 -std=c11 -llua -ldl -lm
	sudo mv spy3 /usr/local/bin
