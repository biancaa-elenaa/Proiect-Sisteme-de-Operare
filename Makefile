CC = gcc

CFLAGS = -g -Wall -O3

TARGET = treasure_manager

RM = rm -f

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) -c treasure.c
	$(CC) $(CFLAGS) -c treasure_manager.c
	$(CC) $(CFLAGS) -o $(TARGET) treasure.o treasure_manager.c -lm

clean:
	$(RM) $(TARGET)