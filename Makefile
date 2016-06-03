LIB=libts
SOURCES=buf.c delta.c compress.c
OBJS=$(SOURCES:.c=.o)
CFLAGS=-Wall -Werror
CC?=clang
RM?=rm -f

all: $(OBJS)
	@echo "done."

.c.o: $<
	$(CC) $(CFLAGS) -c $*.c

clean:
	$(RM) $(OBJS)
