# GDB initialization file
# This file is automatically loaded when GDB starts

# Enable pretty printing for C structures
set print pretty on

# Print arrays in a more readable format
set print array on
set print array-indexes on

# Show the current source code context
set listsize 20

# Don't stop for signals (useful for debugging)
handle SIGUSR1 nostop noprint
handle SIGUSR2 nostop noprint

# Show disassembly in Intel syntax (more readable)
set disassembly-flavor intel

# Automatically display the current line
define hook-stop
    frame
end

# Useful aliases
define c
    continue
end
document c
Continue execution
end

define n
    next
end
document n
Step to next line (step over)
end

define s
    step
end
document s
Step into function
end

define p
    print
end
document p
Print variable value
end
