# Our bank construction team
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
SRCDIR = src
BINDIR = bin
OBJDIR = obj

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/bankSim

# Build the entire bank
all: directories $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ -lm

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Prepare the bank building
directories:
	mkdir -p $(BINDIR) $(OBJDIR) output

# Clean up after hours
clean:
	rm -rf $(BINDIR) $(OBJDIR) output/*

# Test different bank scenarios
test: all
	@echo "Testing Bank Scenarios..."
	./$(TARGET) 100 4 60 2.3
	./$(TARGET) 50 2 30 1.5
	./$(TARGET) 200 8 120 3.0

.PHONY: all clean test directories