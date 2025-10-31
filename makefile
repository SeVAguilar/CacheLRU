CC = gcc
INCDIR = incs
SRCDIR = src
BINDIR = bin
CFLAGS = -I$(INCDIR) -Wall -Wextra -std=c11 -pedantic
LDFLAGS =

SOURCES = $(wildcard $(SRCDIR)/*.c)
TARGET = $(BINDIR)/lru

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SOURCES)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BINDIR) *.o