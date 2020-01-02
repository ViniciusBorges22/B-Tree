compiler = gcc
flags = -Wall -Wextra -Wpedantic -Werror

source_dir = source
include_dir = header

headers = $(include_dir)/*.h

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
