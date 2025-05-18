CC = gcc
CFLAGS = -g -Wall -O3
RM = rm -f

# Executabile
TARGETS = treasure_hub monitor score_calculator

all: $(TARGETS)

# Compilare treasure_hub
treasure_hub: treasure_hub.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

# Compilare monitor
monitor: monitor.o
	$(CC) $(CFLAGS) -o $@ $^

# Compilare score_calculator
score_calculator: score_calculator.o
	$(CC) $(CFLAGS) -o $@ $^

# Reguli implicite pentru .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	$(RM) *.o $(TARGETS)