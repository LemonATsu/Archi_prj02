# Makefile for project 1

CC = cc
CFLAGS = -c -Wall
LDFLAGS = 
SOURCES = data.c main.c reader.c hazard.c instrc.c execute.c  error.c register.c alu.c
OBJECTS = $(SOURCES:.c = .o)
EXECUTABLE = pipeline

# Include .h file here
INCLUDES = obj.h excute.h hazard.h register.h alu.h error.h
all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
%.o: %.c ${INCLUDES}
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE) *.bin *.rpt
