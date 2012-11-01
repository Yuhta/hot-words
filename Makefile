CFILES = $(wildcard src/*.c)
OBJFILES = $(notdir $(CFILES:%.c=%.o))
CFLAGS = -Wextra -Wall -O2 -g -Iinclude

vpath %.c src
vpath %.h include

hot-words: $(OBJFILES)

hot-words.o find-hot-words.o: find-hot-words.h
hot-words.o find-hot-words.o error.o: error.h

.PHONY: clean
clean:
	$(RM) $(OBJFILES) hot-words