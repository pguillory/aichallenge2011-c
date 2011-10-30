CC=gcc
CFLAGS=-O3 -funroll-loops -c -Wall
LDFLAGS=-O2 -lm
SOURCES=MyBot.c globals.c map.c holy_ground.c threat.c mystery.c aroma.c directions.c bot.c server.c handler.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=MyBot

all: $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f ${EXECUTABLE} ${OBJECTS} *.d
	rm -f log.txt

.PHONY: all clean
