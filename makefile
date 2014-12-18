CC=gcc
CFLAGS=-c -std=c99
LDFLAGS=
SOURCES=compression.c PriorityQueue.c BinaryTree.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=compression

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@