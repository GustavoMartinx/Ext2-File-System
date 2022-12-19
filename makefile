CC=gcc -Wall

PROGS=$(super)

all: $(PROGS)

clean:
	rm -f $(PROGS)

super: ext2.c utils.h
	$(CC) ext2.c -o super

debug:
	$(CC) ext2.c -o super
	./super
	rm -f $(PROGS)
