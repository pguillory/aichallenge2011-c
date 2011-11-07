CC=gcc
CFLAGS=-O3 -funroll-loops -c -Wall -DDEBUG
LDFLAGS=-O2 -lm
SOURCES=globals.c map.c potential_enemy.c holy_ground.c army.c threat.c mystery.c aroma.c moves.c bot.c server.c handler.c MyBot.c
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
