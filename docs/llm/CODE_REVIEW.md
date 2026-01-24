# Code Review Guide - Senior Engineer Checklist

This document outlines the systematic approach to reviewing C code for production readiness, focusing on defensive programming, memory safety, and best practices.

## Review Process

### 1. **Memory Safety & Resource Management**

#### Check for:
- ✅ All `malloc()`/`calloc()` calls have corresponding `free()`
- ✅ All SDL resource creation has corresponding destruction
- ✅ No use-after-free (pointers set to NULL after free)
- ✅ No double-free (check for NULL before freeing)
- ✅ No memory leaks (especially in error paths)
- ✅ Proper cleanup in all exit paths (including `goto error`)

#### Red Flags:
- ❌ `malloc()` without matching `free()`
- ❌ Resource creation without cleanup
- ❌ Dereferencing pointers after `free()`
- ❌ Missing cleanup in error paths

### 2. **NULL Pointer Safety**

#### Check for:
- ✅ NULL checks before dereferencing pointers
- ✅ NULL checks after allocations (`malloc`, `SDL_Create*`)
- ✅ NULL checks in public API functions
- ✅ Safe handling of NULL in cleanup functions

#### Red Flags:
- ❌ Dereferencing without NULL check
- ❌ Using return value from allocation without checking
- ❌ Passing potentially NULL pointers to functions expecting non-NULL

### 3. **Array Bounds & Buffer Safety**

#### Check for:
- ✅ Array bounds validation before access
- ✅ Buffer size validation in string operations
- ✅ `snprintf` return value checked for truncation
- ✅ Enum values match array sizes (no off-by-one errors)

#### Red Flags:
- ❌ Array access without bounds checking: `array[index]` where `index` not validated
- ❌ Fixed-size buffers with no overflow protection
- ❌ Enum with `COUNT` but array size is `COUNT - 1` (or vice versa)
- ❌ String operations without size limits

### 4. **Input Validation & Defensive Programming**

#### Check for:
- ✅ All public API functions validate inputs
- ✅ Range checks for numeric parameters (e.g., `> 0`, `< MAX`)
- ✅ Enum value validation (check against `COUNT`)
- ✅ Division by zero prevention (check divisor != 0)

#### Red Flags:
- ❌ Using `assert()` for input validation (removed in release builds!)
- ❌ No validation of user-provided parameters
- ❌ Division/modulo without checking divisor
- ❌ Array indexing without bounds checks

### 5. **Error Handling**

#### Check for:
- ✅ Functions return error codes or NULL on failure
- ✅ Error paths are tested and handled
- ✅ No silent failures (log errors appropriately)
- ✅ Graceful degradation (don't crash on non-fatal errors)

#### Red Flags:
- ❌ Ignoring return values from system/library calls
- ❌ No error handling in critical paths
- ❌ Crashes on expected error conditions

### 6. **Type Safety & Correctness**

#### Check for:
- ✅ Appropriate use of `const` for read-only data
- ✅ Correct types (`size_t` for sizes, `uint8_t` for small values)
- ✅ No implicit conversions that lose precision
- ✅ Proper casting when necessary (with validation)

#### Red Flags:
- ❌ Using `int` for sizes when `size_t` is appropriate
- ❌ Unsafe casts (e.g., `float` to `int` without validation)
- ❌ Mixing signed/unsigned in comparisons

### 7. **Code Organization & Maintainability**

#### Check for:
- ✅ Clear separation of concerns
- ✅ Functions have single responsibility
- ✅ No excessive nesting or complexity
- ✅ Meaningful variable/function names
- ✅ Comments explain "why", not "what"

#### Red Flags:
- ❌ Functions doing too many things
- ❌ Deeply nested conditionals
- ❌ Magic numbers (use named constants)
- ❌ Unclear or misleading names

### 8. **Compiler Warnings & Standards Compliance**

#### Check for:
- ✅ Code compiles with `-Wall -Wextra -Wpedantic`
- ✅ No undefined behavior
- ✅ Follows C11 standard (or specified standard)
- ✅ No compiler-specific extensions (unless documented)

#### Red Flags:
- ❌ Warnings ignored or suppressed without good reason
- ❌ Undefined behavior (signed overflow, etc.)
- ❌ Non-portable code without documentation

## Common Patterns to Look For

### ❌ BAD: Using `assert()` for Input Validation
```c
// BAD - removed in release builds!
void my_function(int* ptr) {
    assert(ptr != NULL);  // ❌ Disappears with -DNDEBUG
    // ... use ptr ...
}
```

### ✅ GOOD: Explicit Runtime Checks
```c
// GOOD - works in all builds
void my_function(int* ptr) {
    if (ptr == NULL) {
        return;  // or return error code
    }
    // ... use ptr ...
}
```

### ❌ BAD: Array Bounds Issue
```c
enum { FIRST, SECOND, THIRD, COUNT };
int array[COUNT - 1];  // ❌ Only 2 elements, but COUNT = 3
array[THIRD] = 5;     // ❌ Out of bounds!
```

### ✅ GOOD: Correct Array Sizing
```c
enum { FIRST, SECOND, THIRD, COUNT };
int array[COUNT];      // ✅ 3 elements, indices 0-2
if (index < COUNT) {  // ✅ Validate before access
    array[index] = 5;
}
```

### ❌ BAD: Missing NULL Check After Allocation
```c
SpriteSheet* sheet = sprite_sheet_new(...);
sheet->frame_count = 10;  // ❌ Crashes if sheet is NULL!
```

### ✅ GOOD: Defensive Check
```c
SpriteSheet* sheet = sprite_sheet_new(...);
if (sheet == NULL) {
    // Handle error
    return;
}
sheet->frame_count = 10;  // ✅ Safe
```

### ❌ BAD: Buffer Overflow Risk
```c
char path[256];
sprintf(path, "%s/%s.png", dir, name);  // ❌ No size limit!
```

### ✅ GOOD: Safe String Operations
```c
char path[256];
int written = snprintf(path, sizeof(path), "%s/%s.png", dir, name);
if (written < 0 || (size_t)written >= sizeof(path)) {
    // Handle truncation
    path[sizeof(path) - 1] = '\0';
}
```

## Review Checklist Template

Use this checklist for each function/file:

- [ ] **Memory Safety**: All allocations freed, no leaks
- [ ] **NULL Safety**: All pointers checked before use
- [ ] **Bounds Safety**: All array accesses validated
- [ ] **Input Validation**: All parameters validated (not just `assert()`)
- [ ] **Error Handling**: Errors handled gracefully
- [ ] **Type Safety**: Correct types used, no unsafe casts
- [ ] **Division Safety**: No division by zero
- [ ] **Buffer Safety**: String operations are bounded
- [ ] **Resource Cleanup**: All resources freed in all paths
- [ ] **Documentation**: Functions have clear purpose

## Priority Levels

### 🔴 CRITICAL (Must Fix Before Commit)
- Memory leaks or use-after-free
- Buffer overflows
- NULL pointer dereferences
- Division by zero
- Array bounds violations
- Using `assert()` for input validation in public APIs

### 🟡 MEDIUM (Should Fix Soon)
- Missing error handling
- Resource leaks in error paths
- Missing input validation
- Type mismatches
- Magic numbers

### 🟢 LOW (Nice to Have)
- Code style inconsistencies
- Missing documentation
- Performance optimizations
- Code duplication

## Questions to Ask During Review

1. **What happens if this function receives NULL?**
2. **What happens if this allocation fails?**
3. **What happens if this array index is out of bounds?**
4. **What happens if this division has a zero divisor?**
5. **Are all error paths handled?**
6. **Are all resources cleaned up?**
7. **Does this work in release builds?** (not just debug)
8. **Is this safe if called from multiple threads?** (if applicable)
9. **What's the worst-case input that could break this?**
10. **Would this pass a security audit?**

## Final Verification

Before approving code:
- ✅ Compiles with `-Wall -Wextra -Wpedantic` with no errors
- ✅ All critical issues addressed
- ✅ Tested with invalid inputs (NULL, out of bounds, etc.)
- ✅ Memory checked with valgrind (if applicable)
- ✅ No undefined behavior
- ✅ Defensive programming in place

## Remember

> **Defensive programming > Static analysis hints**
> 
> Runtime safety checks are more valuable than `__attribute__((nonnull))` annotations. If you're checking for NULL at runtime, remove the attribute to avoid false warnings.

> **Production code must work in release builds**
> 
> Never use `assert()` for input validation in public APIs. Asserts are removed with `-DNDEBUG` in release builds, leaving your code vulnerable.

> **Assume the worst**
> 
> Always assume functions will be called incorrectly, with NULL pointers, invalid indices, or malicious input. Your code should handle these gracefully.
