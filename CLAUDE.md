# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Repository Overview

This is a 42 School Exam04 practice repository containing algorithmic and systems programming challenges organized by difficulty levels. The codebase consists of C programming exercises with increasing complexity across three levels.

## Architecture

### Level Structure
- **Level 1**: Basic algorithms (n_queens, permutation, power_set, rip, tsp)
- **Level 2**: Systems programming (ft_popen, picoshell, sandbox) 
- **Level 3**: Parser implementations (argo JSON parser, vbc expression calculator)

### Code Organization
- Each exercise is self-contained in its own directory with:
  - `subject.txt` - Problem specification and requirements
  - `*.c` files - Implementation
  - `given.c` files (where applicable) - Provided helper code/headers
- `study/` directory contains detailed explanations with comments in Japanese

## Common Development Commands

### Compilation
Most exercises compile as standalone programs:
```bash
# Standard compilation with strict flags
gcc -Wall -Wextra -Werror -o program_name source.c

# For exercises with given files (e.g., argo, vbc)
gcc -Wall -Wextra -Werror -o program_name source.c given.c
```

### Testing
Run compiled programs directly with test inputs:
```bash
# Example: n_queens with size 4
./n_queens 4

# Example: JSON parsing with argo
echo '{"key": "value"}' | ./argo /dev/stdin
```

## Key Technical Constraints

### Allowed Functions
Each exercise has strict allowed function restrictions defined in `subject.txt`. Common patterns:
- **Level 1**: Basic I/O and memory (atoi, fprintf, malloc, free, etc.)
- **Level 2**: System calls (pipe, fork, dup2, execvp, signal handling)
- **Level 3**: File I/O and parsing utilities (getc, ungetc, isdigit, etc.)

### Implementation Requirements
- Pure C implementation without external libraries
- Memory leak prevention is critical
- Error handling must match exact specifications
- Output format must precisely match expected format

## Code Patterns

### Algorithms (Level 1)
- Heavy use of backtracking and recursion
- Dynamic memory allocation with careful cleanup
- Combinatorial problem solving approaches

### Systems Programming (Level 2) 
- Process creation and management with fork/exec
- Inter-process communication via pipes
- File descriptor management and cleanup
- Signal handling and timeouts

### Parsing (Level 3)
- Recursive descent parsing techniques  
- AST (Abstract Syntax Tree) construction
- Token-based lexical analysis
- Error reporting with specific message formats

## Development Notes

- Solutions should be compact and efficient
- Focus on correctness over optimization
- Test with edge cases and invalid inputs
- Each exercise is independent - no shared dependencies
- The `study/` directory contains detailed Japanese explanations for reference