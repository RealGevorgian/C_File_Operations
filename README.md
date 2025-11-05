# Operating Systems – C File Operations 

Low-level file operations using POSIX system calls (`open`, `read`, `write`, `lseek`, `ftruncate`, etc.) on Ubuntu.

## Tasks

| Task | Objective |
|------|-----------|
| 1 | Copy file contents using read/write loops |
| 2 | Demonstrate file truncation and size verification |
| 3 | Print file bytes in reverse using `lseek` |
| 4 | Append log entry with PID and show final offset |
| 5 | Create a sparse file (1 MiB hole) |
| 6 | In-place overwrite of line 4 in a numbers file |
| 7 | Byte-wise file comparison with difference index |

## Build

```bash
make            # build all tasks
make clean      # remove binaries and generated files
