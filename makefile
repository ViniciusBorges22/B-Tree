all: file.exe

file.exe: main.c
	gcc -Wall -Wpedantic -Wextra -o arvore-b main.c arvore_b.c dados.c tad_fila.c -g

clean:
	rm arvore-b
