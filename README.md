# Operating Systems – C File Operations

This project demonstrates low-level file manipulation in C using POSIX system calls such as `open`, `read`, `write`, `lseek`, and `ftruncate`. It was developed and tested on Ubuntu.

**Repository:** https://github.com/RealGevorgian/C_File_Operations.git

## Project Structure

```
C_File_Operations/
├── README.md
└── src/
    ├── task1_copy.c        # Copy file using read/write loops
    ├── task2_truncate.c    # Truncate file and verify size
    ├── task3_reverse.c     # Print file bytes in reverse
    ├── task4_append.c      # Append with PID and show offset
    ├── task5_sparse.c      # Create sparse file with 1 MiB hole
    ├── task6_overwrite.c   # In-place line overwrite with lseek
    └── task7_compare.c     # Byte-wise file comparison
```

## Tasks

| Task | Objective |
|------|------------|
| 1 | Copy file contents using read/write loops |
| 2 | Demonstrate file truncation and size verification |
| 3 | Print file bytes in reverse using `lseek` |
| 4 | Append log entry with PID and show final offset |
| 5 | Create a sparse file (1 MiB hole) |
| 6 | In-place overwrite of line 4 in a numbers file |
| 7 | Byte-wise file comparison with difference index |

## Task Descriptions

### Task 1 — Copy File (`task1_copy.c`)
Copies data from one file to another using a fixed-size buffer and system calls.

### Task 2 — Truncate File (`task2_truncate.c`)
Truncates a file to a given size using `ftruncate()` and verifies the result with `stat()`.

### Task 3 — Reverse File (`task3_reverse.c`)
Reads a file from end to start using `lseek()` and writes it reversed to a new file.

### Task 4 — Append Log (`task4_append.c`)
Appends a log entry containing the process ID and displays the new file offset.

### Task 5 — Sparse File (`task5_sparse.c`)
Creates a sparse file with a large hole (1 MiB) using `lseek()` to skip empty regions.

### Task 6 — Overwrite Line (`task6_overwrite.c`)
Overwrites line 4 of a file containing numbers without rewriting the entire file.

### Task 7 — Compare Files (`task7_compare.c`)
Compares two files byte-by-byte and prints the first differing byte index, if any.



**Generated files** (cleaned by `make clean`):
- `data.txt` | `log.txt` | `sparse.bin` | `numbers.txt`

---

## Task-by-Task Summary

---

### **Task 1: Copy file contents**
**Objective**: Copy full file using `read`/`write` loops
**Key Features**:
- Prompts for **source** and **destination** paths
- Opens source: `O_RDONLY`
- Opens dest: `O_WRONLY|O_CREAT|O_TRUNC`, mode `0644`
- Counts and prints **total bytes copied**
- Robust partial write handling

---

### **Task 2: File truncation**
**Objective**: Write, truncate, verify size/content
**Key Features**:
- Fixed file: `data.txt`
- Writes `ABCDEFGHIJKLMNOPQRSTUVWXYZ` (26 bytes)
- Reopens with `O_RDWR`
- Uses `lseek(fd, 0, SEEK_END)` → prints **26**
- `ftruncate(fd, 10)`
- Prints new size: **10**
- Rewinds and prints: `ABCDEFGHIJ`

---

### **Task 3: Reverse file reader**
**Objective**: Print file bytes in reverse using `lseek`
**Key Features**:
- Prompts for file path
- Opens `O_RDONLY`
- Gets size via `lseek(..., SEEK_END)`
- Loops from `size-1` → `0`:
  - `lseek(position)`
  - `read(1 byte)`
  - `write(STDOUT_FILENO)`
- Handles **empty file** → prints only `\n`

---

### **Task 4: Append log entries with PID and final offset**
**Objective**: Append with `O_APPEND`, show final offset
**Key Features**:
- Fixed file: `log.txt`
- Opens: `O_WRONLY|O_CREAT|O_APPEND`, `0644`
- Uses `read(0, ...)` — **no `fgets`**
- Prepends `PID=<pid>: `
- Ensures trailing `\n`
- Prints final offset via `lseek(fd, 0, SEEK_CUR)`

> **Comment in code**:
> ```c
> /* O_APPEND forces writes to EOF, but the file descriptor's offset still grows.
>    lseek(fd, 0, SEEK_CUR) returns where the next write would occur if O_APPEND
>    were not set. This offset increases after each successful append. */
> ```

---

### **Task 5: Sparse file creator**
**Objective**: Create sparse file with 1 MiB hole
**Key Features**:
- Fixed file: `sparse.bin`
- Opens: `O_WRONLY|O_CREAT|O_TRUNC`, `0644`
- Writes `"START"`
- `lseek(fd, 1024*1024, SEEK_CUR)` → **hole**
- Writes `"END"`
- Reopens `O_RDONLY`, prints size: **≥ 1,048,581**
- **No manual zero writes**

> **Comment in code**:
> ```c
> /* => The 1 MiB gap is a "hole" — no physical blocks allocated.
>    Filesystem stores only metadata + actual data ("START"+"END" = 8 bytes).
>    Thus, `du` shows ~8K, while `ls` or SEEK_END shows ~1.05 MiB. */
> ```

---

### **Task 6: In-place overwrite using lseek**
**Objective**: Replace line 4 (`4` → `100`) in-place
**Key Features**:
- Fixed file: `numbers.txt`
- Writes `1\n` through `10\n`
- Reopens `O_RDWR`
- **Scans file**, counts newlines → finds **exact byte offset** of line 4
- Reads **remainder** into memory
- Overwrites with `"100\n"`
- Writes back remainder
- Uses `ftruncate()` if file shrinks
- Prints final content

---

### **Task 7: Byte-wise file comparator**
**Objective**: Find first differing byte or EOF
**Key Features**:
- Prompts for **two file paths**
- Opens both `O_RDONLY`
- Compares in **4KB chunks**
- **Byte-by-byte** within chunks
- Reports:
  - `Files differ at byte N` (0-based)
  - Or `Files are identical`
- Handles **shorter file** → difference at EOF
- **Non-zero exit** on difference

---

## Build & Run

```bash
make                 # Build all
./task1_copy         # Run any task
make clean           # Remove binaries + generated files
```

---

## Alternatively, compile individual tasks manually:

```bash
gcc src/task1_copy.c -o bin/task1_copy
gcc src/task2_truncate.c -o bin/task2_truncate
...
```

---

## Usage Examples

```bash
./task1_copy input.txt output.txt
./task2_truncate file.txt 100
./task3_reverse input.txt reversed.txt
./task4_append log.txt
./task5_sparse sparse.bin
./task6_overwrite numbers.txt
./task7_compare file1.txt file2.txt
```
