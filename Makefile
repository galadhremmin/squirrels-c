# Makefile for SDL C project (macOS and Fedora Linux compatible)

# Detect OS
UNAME_S := $(shell uname -s)

# Compilers
CC = gcc
CXX = g++

# Compiler flags
CFLAGS = -Wall -Wextra -std=c11 -O2
CXXFLAGS = -Wall -Wextra -Wpedantic -std=c++23 -O2

# Debug flags (for development)
DEBUG_CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -g -O0 -DDEBUG
DEBUG_CXXFLAGS = -Wall -Wextra -Wpedantic -std=c++23 -g -O0 -DDEBUG
SANITIZER_CFLAGS = $(DEBUG_CFLAGS) -fsanitize=address -fsanitize=undefined
SANITIZER_CXXFLAGS = $(DEBUG_CXXFLAGS) -fsanitize=address -fsanitize=undefined

# SDL configuration
ifeq ($(UNAME_S),Darwin)
    # macOS
    SDL_CFLAGS = $(shell pkg-config --cflags sdl3 sdl3-image)
    SDL_LIBS = $(shell pkg-config --libs sdl3 sdl3-image)
else
    # Linux (Fedora)
    SDL_CFLAGS = $(shell pkg-config --cflags sdl3 sdl3-image)
    SDL_LIBS = $(shell pkg-config --libs sdl3 sdl3-image)
endif

# Target executable
TARGET = squirrel

# Output directory for executable and shared libraries
BINDIR = bin

# Source directory
SRCDIR = src

# Automatically find all .c/.cpp files recursively
SOURCES_C = $(shell find $(SRCDIR) -name "*.c" -type f | sort)
SOURCES_CPP = $(shell find $(SRCDIR) -name "*.cpp" -type f | sort)
SOURCES = $(SOURCES_C) $(SOURCES_CPP)

# Generate object file names from source files (in BINDIR)
# src/main.c -> bin/main.o, src/sprites/sprite.c -> bin/sprite.o
OBJECTS_C = $(foreach src,$(SOURCES_C),$(BINDIR)/$(notdir $(src:.c=.o)))
OBJECTS_CPP = $(foreach src,$(SOURCES_CPP),$(BINDIR)/$(notdir $(src:.cpp=.o)))
OBJECTS = $(OBJECTS_C) $(OBJECTS_CPP)

# Default target
all: $(BINDIR)/$(TARGET)

# Ensure output directory exists
$(BINDIR):
	mkdir -p $(BINDIR)

# Build target
$(BINDIR)/$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(BINDIR)/$(TARGET) $(SDL_LIBS)

# Automatic rule: compile any .c/.cpp file to .o in BINDIR
# Uses vpath to search for source files in subdirectories
vpath %.c $(SRCDIR) $(shell find $(SRCDIR) -type d)
vpath %.cpp $(SRCDIR) $(shell find $(SRCDIR) -type d)

# Pattern rule: any .o in BINDIR can be built from corresponding .c
$(BINDIR)/%.o: %.c | $(BINDIR)
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -c $< -o $@

# Pattern rule: any .o in BINDIR can be built from corresponding .cpp
$(BINDIR)/%.o: %.cpp | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(SDL_CFLAGS) -c $< -o $@

# Clean build artifacts (object files, executable, and any .so in BINDIR)
clean:
	rm -f *.o $(OBJECTS) $(BINDIR)/$(TARGET) $(BINDIR)/*.so

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
	@echo "Install SDL3 and SDL3_image development packages (pkg-config is usually included):"
	@echo "  sudo dnf install SDL3-devel SDL3_image-devel pkg-config"
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

# Check if SDL3 and SDL3_image are installed
check-deps: check-pkg-config
	@echo "Checking for SDL3..."
	@pkg-config --exists sdl3 && echo "SDL3 found!" || (echo "SDL3 not found! Run 'make install-deps' for instructions." && exit 1)
	@echo "Checking for SDL3_image..."
	@pkg-config --exists sdl3-image && echo "SDL3_image found!" || (echo "SDL3_image not found! Run 'make install-deps' for instructions." && exit 1)

# Debug build (with debugging symbols, no optimization)
debug: CFLAGS = $(DEBUG_CFLAGS)
debug: CXXFLAGS = $(DEBUG_CXXFLAGS)
debug: $(BINDIR)/$(TARGET)

# Debug build with sanitizers (catches memory errors at runtime)
sanitize: CFLAGS = $(SANITIZER_CFLAGS)
sanitize: CXXFLAGS = $(SANITIZER_CXXFLAGS)
sanitize: $(BINDIR)/$(TARGET)

# Run the program
run: $(BINDIR)/$(TARGET)
	./$(BINDIR)/$(TARGET)

# Run with valgrind (memory leak checker)
valgrind: $(BINDIR)/$(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(BINDIR)/$(TARGET)

# Static analysis with cppcheck
check:
	cppcheck --enable=all --inline-suppr --suppress=missingIncludeSystem $(SRCDIR)/

# Format code with clang-format (if installed)
# Automatically finds all .c/.cpp and .h/.hpp files
format:
	@find $(SRCDIR) -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | xargs clang-format -i

# Check line length (warn about lines > 100 chars)
check-lines:
	@echo "Checking for lines longer than 100 characters..."
	@find $(SRCDIR) -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | xargs awk 'length > 100 {print FILENAME":"NR": "length" chars: "$$0}' || true

# Phony targets
.PHONY: all clean install-deps check-deps check-pkg-config run debug sanitize valgrind check format
