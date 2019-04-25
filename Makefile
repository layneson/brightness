brightness: main.c
	gcc -o $@ -Wall -g $^

.PHONY: install
install: brightness
	cp brightness ~/bin/
