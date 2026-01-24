# Makefile for SDL C project (macOS and Fedora Linux compatible)

# Detect OS
UNAME_S := $(shell uname -s)

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c11 -O2

# SDL configuration
ifeq ($(UNAME_S),Darwin)
    # macOS
    SDL_CFLAGS = $(shell pkg-config --cflags sdl3)
    SDL_LIBS = $(shell pkg-config --libs sdl3)
else
    # Linux (Fedora)
    SDL_CFLAGS = $(shell pkg-config --cflags sdl3)
    SDL_LIBS = $(shell pkg-config --libs sdl3)
endif

# Target executable
TARGET = sdl_example

# Source files
SOURCES = main.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(SDL_LIBS)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)

# Install dependencies (platform-specific)
install-deps:
ifeq ($(UNAME_S),Darwin)
	@echo "Installing dependencies on macOS..."
	@echo ""
	@echo "1. Install pkg-config (if not already installed):"
	@echo "   brew install pkg-config"
	@echo ""
	@echo "2. Install SDL3:"
	@echo "   brew install sdl3"
else
	@echo "Installing dependencies on Fedora Linux..."
	@echo ""
	@echo "Install SDL3 development packages (pkg-config is usually included):"
	@echo "  sudo dnf install SDL3-devel pkg-config"
endif

# Check if pkg-config is available
check-pkg-config:
	@which pkg-config > /dev/null 2>&1 || ( \
		echo "Error: pkg-config is not installed!" && \
		if [ "$(UNAME_S)" = "Darwin" ]; then \
			echo "On macOS, install it with: brew install pkg-config"; \
		else \
			echo "On Fedora, install it with: sudo dnf install pkg-config"; \
		fi && \
		exit 1 \
	)

# Check if SDL3 is installed
check-deps: check-pkg-config
	@echo "Checking for SDL3..."
	@pkg-config --exists sdl3 && echo "SDL3 found!" || (echo "SDL3 not found! Run 'make install-deps' for instructions." && exit 1)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Phony targets
.PHONY: all clean install-deps check-deps check-pkg-config run
