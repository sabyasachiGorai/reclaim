Good. **Phase 1 — Step 1 is complete.** You successfully verified recursive traversal, so now we can build on it.

# Phase 1 — Step 2

## `directory_entry`: Files vs Directories + Safe Filtering

Now we're going to answer an important question:

> **When `recursive_directory_iterator` gives us an entry, how do we know whether it's actually a file that Reclaim should process?**

This is the foundation for everything that comes next.

---

# 1. 🧠 What is `directory_entry`?

When we wrote:

```cpp
for (const auto& entry :
     std::filesystem::recursive_directory_iterator(root))
```

`entry` isn't a string.

It is essentially a:

```cpp
std::filesystem::directory_entry
```

A `directory_entry` represents one filesystem entry.

It can represent:

```text
file
directory
symbolic link
```

and provides information about that entry.

For example:

```cpp
entry.path()
```

gives us its path.

We can also ask:

```cpp
entry.is_regular_file()
```

or:

```cpp
entry.is_directory()
```

---

# 2. Why do we need `is_regular_file()`?

Reclaim eventually wants to find duplicate **files**.

Suppose we have:

```text
Downloads/
├── movie.mp4
├── photo.jpg
├── Projects/
└── Documents/
```

The iterator encounters:

```text
movie.mp4       → file
photo.jpg       → file
Projects/       → directory
Documents/      → directory
```

We don't want to calculate a file size or hash for:

```text
Projects/
```

because it's a directory, not a regular file.

So eventually:

```cpp
if (entry.is_regular_file()) {
    // process this file
}
```

---

# 3. What is a regular file?

For our purposes, think of a regular file as the normal kind of filesystem object containing data:

```text
.txt
.jpg
.mp4
.pdf
.cpp
.zip
...
```

This is different from:

```text
directory
symbolic link
device
socket
...
```

The exact filesystem object types can vary between operating systems, but for Reclaim our primary target is:

```text
regular file
```

---

# 4. `is_directory()`

We can also check:

```cpp
entry.is_directory()
```

which tells us whether the entry is a directory.

For example:

```cpp
if (entry.is_directory()) {
    std::cout << "Directory: "
              << entry.path() << '\n';
}
```

---

# 5. Let's modify our scanner

Open:

```text id="9lrxqf"
Reclaim/src/main.cpp
```

Replace it with:

```cpp
#include <iostream>
#include <filesystem>

int main() {
    std::filesystem::path root = ".";

    for (const auto& entry :
         std::filesystem::recursive_directory_iterator(root)) {

        if (entry.is_regular_file()) {
            std::cout << "FILE: "
                      << entry.path()
                      << '\n';
        }
        else if (entry.is_directory()) {
            std::cout << "DIRECTORY: "
                      << entry.path()
                      << '\n';
        }
    }

    return 0;
}
```

---

# 6. Build

From:

```text
reclaim/build/
```

run:

```bash
cmake --build .
```

Then:

```bash
./reclaim.exe
```

Now instead of simply seeing:

```text
"./CMakeCache.txt"
"./CMakeFiles"
```

you should see:

```text
FILE: "./CMakeCache.txt"
DIRECTORY: "./CMakeFiles"
FILE: "./Makefile"
...
```

The exact output will depend on your build directory.

---

# 7. What is actually happening?

The loop:

```cpp
for (const auto& entry :
     std::filesystem::recursive_directory_iterator(root))
```

does the traversal.

For every filesystem entry:

```text
             entry
               │
       ┌───────┴────────┐
       ▼                ▼
 regular file        directory
       │                │
       ▼                ▼
 process it        traversal continues
```

Our `if` statements simply classify what the iterator gives us.

---

# 8. Important: the directory iterator handles recursion

You might wonder:

> "If we're checking `is_directory()`, how does the iterator enter the directory?"

That's the job of:

```cpp
recursive_directory_iterator
```

The iterator itself handles recursion.

We're only **classifying** the entries.

For example:

```text
Downloads/
│
├── file.txt
│
└── Projects/
    │
    ├── a.cpp
    └── b.cpp
```

The iterator can visit:

```text
file.txt
Projects/
Projects/a.cpp
Projects/b.cpp
```

We don't manually write:

```cpp
for each directory
    open directory
    for each subdirectory
        ...
```

The filesystem iterator handles that traversal.

---

# 9. Interview Questions

## Q1. What does `recursive_directory_iterator` return?

> It iterates over filesystem entries, represented by `std::filesystem::directory_entry`. An entry can represent a file, directory, symbolic link, or other filesystem object.

---

## Q2. How do you determine whether an entry is a regular file?

> We can use `entry.is_regular_file()`.

---

## Q3. How do you determine whether an entry is a directory?

> We can use `entry.is_directory()`.

---

## Q4. Why shouldn't we treat every directory entry as a file?

> Because a directory entry can represent different filesystem object types. Reclaim needs to perform file-specific operations such as retrieving file size and eventually hashing file contents, so we need to filter for regular files.

---

## Q5. What's the difference between `path` and `directory_entry`?

This is important.

### `path`

Represents the **location/name**:

```cpp
std::filesystem::path
```

Example:

```text
D:\Downloads\movie.mp4
```

### `directory_entry`

Represents a **filesystem entry** and provides access to its path and metadata/type information.

So:

```text
directory_entry
      │
      ├── path()
      ├── is_regular_file()
      ├── is_directory()
      └── other metadata operations
```

### Interview answer

> "`std::filesystem::path` represents a filesystem path, while `std::filesystem::directory_entry` represents an actual directory entry encountered during filesystem traversal and provides operations for querying that entry."

---

# 10. ⚠️ Important problem we're about to encounter

You might think:

```cpp
entry.is_regular_file()
```

is always safe.

But filesystem operations can fail.

Imagine Reclaim scans:

```text
C:\SomeFolder
```

and encounters a file or directory where the process doesn't have permission to retrieve metadata.

Then filesystem operations can potentially produce errors.

That's why the original Phase 1 plan specifically requires us to handle:

* permission denied
* broken symlinks
* filesystem errors

without crashing the entire program. 

**That's our next step.**

---

# 11. Another important issue: symbolic links

Suppose:

```text
A/
└── link → B/

B/
└── link → A/
```

If a traversal follows symbolic links indiscriminately, you can potentially end up traversing a cycle.

Conceptually:

```text
A
 ↓
B
 ↓
A
 ↓
B
 ↓
...
```

That's why Reclaim needs a deliberate policy regarding symlinks.

For our first implementation, we'll be conservative.

We'll learn:

* what a symlink is
* how `recursive_directory_iterator` treats it
* how to detect one
* why following arbitrary symlinks can be dangerous

**Don't modify the code for this yet.**

---

# 12. 🧪 Your test

Run the program and verify that you can see both:

```text
FILE:
```

and:

```text
DIRECTORY:
```

entries.

Then create a small test directory somewhere, for example:

```text
reclaim_test/
├── file1.txt
├── file2.txt
└── folder/
    └── file3.txt
```

Point:

```cpp
std::filesystem::path root = "...";
```

at that directory and verify that all entries are classified correctly.

Don't use destructive operations. We're only reading.

---

# 📊 Progress

### Phase 1

| Step                                        | Status         |
| ------------------------------------------- | -------------- |
| Step 1 — Recursive traversal                | ✅ Complete     |
| Step 2 — `directory_entry` + file filtering | 🟡 **Current** |
| Step 3 — Errors + permissions + symlinks    | ⬜              |
| Step 4 — `FileInfo` + file size             | ⬜              |
| Step 5 — `vector<FileInfo>`                 | ⬜              |
| Step 6 — Sorting                            | ⬜              |
| Step 7 — Command-line arguments             | ⬜              |
| Step 8 — Final scanner/report               | ⬜              |

### Current progress

**Phase 1: ~20%**

**Overall Reclaim: ~22%**

Once you verify this step works, we'll tackle **Step 3: filesystem errors, permission denied, and symlinks**. This is an especially valuable systems/interview step because we're going to make Reclaim **not crash just because one file can't be accessed**.
