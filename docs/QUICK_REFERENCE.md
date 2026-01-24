# Quick Reference: C Development on Fedora

## Installation (Run Once)

```bash
sudo dnf install -y gcc gcc-c++ make gdb valgrind clang clang-tools-extra cppcheck pkg-config
```

## Common Commands

### Building
```bash
make              # Build release version
make debug        # Build with debug symbols (-g -O0)
make sanitize     # Build with AddressSanitizer and UBSan
make clean        # Remove build artifacts
make run          # Build and run
```

### Debugging with GDB

#### Start debugging:
```bash
gdb ./sdl_example
```

#### Essential GDB commands:
```
(gdb) break main              # Set breakpoint at main()
(gdb) break 42                # Set breakpoint at line 42
(gdb) break function_name     # Set breakpoint at function
(gdb) run                     # Start program
(gdb) continue                # Continue execution (or 'c')
(gdb) next                    # Step over (execute line, don't enter functions)
(gdb) step                    # Step into (enter function calls)
(gdb) finish                  # Step out (run until function returns)
(gdb) print variable          # Print variable value
(gdb) print *pointer          # Print value pointed to
(gdb) print array[0]@10      # Print first 10 elements of array
(gdb) info locals             # Show all local variables
(gdb) info registers          # Show CPU registers
(gdb) backtrace               # Show call stack (or 'bt')
(gdb) frame 2                 # Switch to frame 2 in call stack
(gdb) list                    # Show source code around current line
(gdb) watch variable          # Break when variable changes
(gdb) quit                    # Exit GDB
```

#### VS Code debugging:
- Press `F5` or click "Run and Debug"
- Set breakpoints by clicking left of line numbers
- Use debug panel to inspect variables

### Memory Debugging

#### Valgrind (finds memory leaks and errors):
```bash
make valgrind
# or manually:
valgrind --leak-check=full --show-leak-kinds=all ./sdl_example
```

#### AddressSanitizer (faster, built into program):
```bash
make sanitize
./sdl_example
# Will show memory errors immediately if they occur
```

### Static Analysis

```bash
make check                    # Run cppcheck
clang-tidy main.c            # Run clang-tidy
```

### Code Formatting

```bash
make format                   # Format with clang-format
# or manually:
clang-format -i main.c
```

## Compiler Flags Explained

### Development flags:
- `-Wall`: Enable all common warnings
- `-Wextra`: Enable extra warnings
- `-Wpedantic`: Strict ISO C compliance warnings
- `-std=c11`: Use C11 standard
- `-g`: Include debugging symbols (needed for GDB)
- `-O0`: No optimization (faster compile, easier debugging)
- `-fsanitize=address`: Detect memory errors at runtime
- `-fsanitize=undefined`: Detect undefined behavior

### Release flags:
- `-O2`: Optimize for speed
- `-DNDEBUG`: Disable assert() macros
- Remove `-g` and sanitizers for smaller binary

## Common Issues & Solutions

### "gcc: command not found"
```bash
sudo dnf install gcc
```

### "make: command not found"
```bash
sudo dnf install make
```

### "SDL3 not found" when compiling
```bash
sudo dnf install SDL3-devel pkg-config
```

### Program crashes with "Segmentation fault"
1. Run with GDB to see where it crashes:
   ```bash
   gdb ./sdl_example
   (gdb) run
   ```
2. Check with Valgrind:
   ```bash
   make valgrind
   ```
3. Build with sanitizers:
   ```bash
   make sanitize
   ./sdl_example
   ```

### Memory leaks detected
- Use Valgrind to see exact location
- Check that all `malloc()` have matching `free()`
- Check that all SDL resources are destroyed

## Useful GDB Tips

### Conditional breakpoints:
```
(gdb) break 42 if variable == 5
```

### Print formatted output:
```
(gdb) print/x variable    # Hexadecimal
(gdb) print/d variable    # Decimal
(gdb) print/t variable    # Binary
(gdb) print variable      # Default format
```

### Examine memory:
```
(gdb) x/10x $rsp          # Examine 10 bytes in hex at stack pointer
(gdb) x/s pointer         # Examine as string
```

### Run with arguments:
```
(gdb) set args arg1 arg2
(gdb) run
```

## VS Code Shortcuts

- `F5`: Start debugging
- `F9`: Toggle breakpoint
- `F10`: Step over
- `F11`: Step into
- `Shift+F11`: Step out
- `Ctrl+Shift+B`: Build
- `Ctrl+Shift+P` → "C/C++: Select a Configuration": Choose compiler
