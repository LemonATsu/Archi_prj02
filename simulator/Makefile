# Makefile for project 1

CC = cc
CFLAGS = -c -Wall
LDFLAGS = 
SOURCES = main.c reader.c data.c instrc.c register.c execute.c operator.c error.c
OBJECTS = $(SOURCES:.c = .o)
EXECUTABLE = single_cycle

# Include .h file here
INCLUDES = obj.h excute.h operator.h
all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
%.o: %.c ${INCLUDES}
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE) *.bin *.rpt
