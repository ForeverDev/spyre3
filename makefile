spy3: 
	gcc main.c svm.c -o spy3 -std=c11 -llua -ldl -lm
	sudo mv spy3 /usr/local/bin
