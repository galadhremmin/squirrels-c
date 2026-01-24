# C Development Environment Setup for Fedora

## Essential Tools Installation

Run this command to install all essential C development tools:

```bash
sudo dnf install -y \
    gcc \
    gcc-c++ \
    make \
    gdb \
    valgrind \
    clang \
    clang-tools-extra \
    cppcheck \
    pkg-config
```

## What Each Tool Does

### **GCC** (GNU Compiler Collection)
- The standard C compiler on Linux
- Excellent optimization and standards compliance
- Supports C11, C17, and newer standards
- **Usage**: `gcc -Wall -Wextra -std=c11 -g main.c -o program`

### **GDB** (GNU Debugger)
- The standard debugger for C/C++
- Set breakpoints, inspect variables, step through code
- **Usage**: `gdb ./program` or `gdb --args ./program arg1 arg2`

### **Make**
- Build automation tool
- Reads Makefiles to compile projects
- **Usage**: `make` (builds default target), `make clean` (removes build artifacts)

### **Valgrind**
- Memory error detector and profiler
- Finds memory leaks, use-after-free, buffer overflows
- **Usage**: `valgrind --leak-check=full ./program`

### **Clang**
- Alternative compiler (LLVM-based)
- Often provides better error messages
- **Usage**: `clang -Wall -Wextra -std=c11 -g main.c -o program`

### **clang-tools-extra**
- Includes `clang-tidy` (static analysis) and `clang-format` (code formatting)
- **Usage**: 
  - `clang-tidy main.c` (analyze code)
  - `clang-format -i main.c` (format code)

### **cppcheck**
- Static analysis tool for C/C++
- Finds bugs without running the program
- **Usage**: `cppcheck --enable=all main.c`

## Quick Start Commands

### Compile with debugging symbols:
```bash
gcc -Wall -Wextra -std=c11 -g -O0 main.c -o program
```

### Debug your program:
```bash
gdb ./program
# Inside GDB:
#   (gdb) break main          # Set breakpoint at main()
#   (gdb) run                 # Start program
#   (gdb) next                # Step to next line
#   (gdb) print variable      # Print variable value
#   (gdb) continue            # Continue execution
#   (gdb) quit                # Exit
```

### Check for memory leaks:
```bash
valgrind --leak-check=full --show-leak-kinds=all ./program
```

### Static analysis:
```bash
cppcheck --enable=all --suppress=missingIncludeSystem main.c
clang-tidy main.c -- -std=c11
```

## Recommended Compiler Flags

### Development (with debugging):
```bash
gcc -Wall -Wextra -Wpedantic -std=c11 -g -O0 -fsanitize=address -fsanitize=undefined main.c
```

### Release (optimized):
```bash
gcc -Wall -Wextra -Wpedantic -std=c11 -O2 -DNDEBUG main.c
```

### Flag explanations:
- `-Wall`: Enable all common warnings
- `-Wextra`: Enable extra warnings
- `-Wpedantic`: Strict ISO C compliance
- `-std=c11`: Use C11 standard
- `-g`: Include debugging symbols
- `-O0`: No optimization (faster compilation, easier debugging)
- `-O2`: Optimize for speed
- `-fsanitize=address`: Detect memory errors at runtime
- `-fsanitize=undefined`: Detect undefined behavior

## VS Code Integration

If you're using VS Code, install these extensions:
- **C/C++** (Microsoft) - IntelliSense, debugging, code navigation
- **C/C++ Extension Pack** - Includes multiple C/C++ tools

The `.vscode/launch.json` file is already configured for debugging with GDB.
