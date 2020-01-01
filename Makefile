compiler = gcc
flags = -Wall -Wextra -Wpedantic -Werror

source_dir = source
include_dir = header

#_headers = arvore_b.h dados.h tad_fila.h
headers = $(include_dir)/*.h

#_sources = arvore_b.c dados.c tad_fila.c main.c
sources = $(source_dir)/*.c

executable = music_manager

.PHONY: all
all: $(executable)

.PHONY: run
run: all
	./$(executable)

.PHONY: debug
debug: flags += -g
debug: clean $(executable)

$(executable): $(sources) $(headers)
	$(compiler) $(flags) -I $(include_dir) $(sources) -o $@

.PHONY: clean
clean:
	rm $(executable)
