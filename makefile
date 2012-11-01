OBJ = hot-words.o find-hot-words.o error.o
CFLAGS = -Wextra -Wall -O2

hot-words: $(OBJ)

hot-words.o find-hot-words.o: find-hot-words.h
hot-words.o find-hot-words.o error.o: error.h

.PHONY: clean
clean:
	$(RM) $(OBJ)