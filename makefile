all: file.exe

file.exe: arvore_b.c
	gcc -Wall -Wpedantic -Wextra -o arvore-B.exe main.c arvore_b.c dados.c tad_fila.c

clean:
	rm arvore-B.exe
