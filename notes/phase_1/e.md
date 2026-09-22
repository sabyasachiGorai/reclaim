Good. **Step 5 is complete.** Now we add the next piece of information that every `FileInfo` needs.

# Phase 1 — Step 6: Get File Size

This is an important step because **file size will later become our first duplicate-detection filter**.

If two files have different sizes:

```text
file A → 10 KB
file B → 20 KB
```

they **cannot be identical byte-for-byte**.

So there's no reason to calculate an expensive hash for both.

Later our duplicate pipeline will look roughly like:

```text
All files
   ↓
Group by file size
   ↓
Only same-size files
   ↓
Calculate hashes
   ↓
Same hash
   ↓
Duplicate candidates
```

We're building the first part of that now.

---

# 1. `std::filesystem::file_size()`

C++17 provides:

```cpp
std::filesystem::file_size(path)
```

For example:

```cpp
std::uintmax_t size =
    std::filesystem::file_size(entry.path());
```

If:

```text
test.txt = 1500 bytes
```

then:

```cpp
file_size("test.txt")
```

returns:

```text
1500
```

---

# 2. Update `FileInfo`

Open:

```text
Reclaim/include/FileInfo.h
```

We already have:

```cpp
#pragma once

#include <filesystem>
#include <cstdint>

struct FileInfo {
    std::filesystem::path path;
    std::uintmax_t size;
};
```

The structure is **already ready for size**.

So we don't actually need to change `FileInfo.h`.

That's intentional.

---

# 3. Update the scanner

Open:

```text
Reclaim/src/main.cpp
```

Inside the regular-file section, we currently have:

```cpp
FileInfo info;
info.path = entry.path();

files.push_back(info);
```

Change it to:

```cpp
FileInfo info;
info.path = entry.path();
info.size = std::filesystem::file_size(entry.path(), ec);

if (ec) {
    std::cerr << "Could not get file size for: "
              << entry.path()
              << " - "
              << ec.message()
              << '\n';

    ec.clear();
}
else {
    files.push_back(info);
}
```

So the complete relevant section becomes:

```cpp
if (entry.is_regular_file(ec)) {

    FileInfo info;
    info.path = entry.path();

    info.size = std::filesystem::file_size(
        entry.path(),
        ec
    );

    if (ec) {
        std::cerr << "Could not get file size for: "
                  << entry.path()
                  << " - "
                  << ec.message()
                  << '\n';

        ec.clear();
    }
    else {
        files.push_back(info);
    }
}
```

---

# 4. Why are we passing `ec`?

Notice that we're using:

```cpp
std::filesystem::file_size(
    entry.path(),
    ec
);
```

rather than:

```cpp
std::filesystem::file_size(
    entry.path()
);
```

The second version can throw a `filesystem_error` if something goes wrong.

The first version reports the error through:

```cpp
ec
```

That fits the error-handling approach we established in Step 3.

---

# 5. Why can getting a file size fail?

You might think:

> "The file exists, so why can't we get its size?"

Because filesystem state can change.

For example:

```text
Scanner sees file
       ↓
another process deletes file
       ↓
Reclaim asks for its size
       ↓
file no longer exists
       ↓
ERROR
```

Or:

```text
file exists
   ↓
permission denied
   ↓
cannot retrieve metadata
```

This is why robust filesystem programs don't assume that something they saw a moment ago will still exist.

That's an important real-world systems concept.

---

# 6. Let's print the size

For now, let's make the information visible.

Change:

```cpp
std::cout << "\nFiles found: "
          << files.size()
          << '\n';
```

to:

```cpp
std::cout << "\nFiles found: "
          << files.size()
          << "\n\n";

for (const auto& file : files) {
    std::cout << file.path
              << " | "
              << file.size
              << " bytes\n";
}
```

Now the output should look something like:

```text
Files found: 4

"./src/main.cpp" | 1542 bytes
"./src/FileInfo.cpp" | 823 bytes
"./include/FileInfo.h" | 178 bytes
"./CMakeLists.txt" | 412 bytes
```

Your exact files and sizes will obviously differ.

---

# 7. Important: bytes vs KB/MB

`file_size()` gives us **bytes**.

For example:

```text
1024 bytes = 1 KiB
1024 KiB = 1 MiB
```

For now, **keep the raw byte value**.

Don't convert it to:

```text
1.4 MB
```

inside `FileInfo`.

Why?

Because raw bytes are precise and useful for comparisons.

For example:

```cpp
if (fileA.size == fileB.size)
```

is straightforward.

Formatting into KB/MB should be something the **output layer** handles later.

This is a good separation of data from presentation.

---

# 8. Why size is so important for duplicate detection

Suppose Reclaim finds:

```text
A.txt = 50 KB
B.txt = 50 KB
C.txt = 75 KB
D.txt = 50 KB
```

We can immediately eliminate C:

```text
50 KB → A
50 KB → B
75 KB → C  ← cannot match A/B/D
50 KB → D
```

So instead of hashing:

```text
A
B
C
D
```

we only hash:

```text
A
B
D
```

If we eventually have:

```text
1,000,000 files
```

and only a small fraction share a size, this can significantly reduce unnecessary hashing.

This is called a **prefilter** or **candidate reduction**.

Important:

> Same size does NOT mean duplicate.

Two completely different files can have exactly the same size.

For example:

```text
hello.txt → 1000 bytes
photo.jpg → 1000 bytes
```

They aren't necessarily identical.

So:

```text
same size
    ↓
possible duplicate
```

not:

```text
same size
    ↓
duplicate
```

The hash comes later.

---

# 9. Interview Questions

### Q1. How do you get a file's size in C++17?

**Answer:**

> I use `std::filesystem::file_size()`, which returns the file size as `std::uintmax_t`.

---

### Q2. Why use file size before hashing?

**Answer:**

> Files with different sizes cannot be byte-for-byte identical, so size can be used as a cheap prefilter. Only files with the same size need to proceed to the more expensive hashing stage.

---

### Q3. Does same file size mean two files are duplicates?

**Answer:**

> No. Different files can have the same size. Matching size only makes them candidates; we need a stronger comparison such as a cryptographic hash or byte-by-byte comparison.

---

### Q4. Why store size in `FileInfo`?

**Answer:**

> Size is metadata that we will need in multiple stages, particularly for candidate filtering and reporting. Storing it in `FileInfo` means we don't have to repeatedly query the filesystem.

That's an important design benefit.

---

### Q5. Why don't we store formatted strings such as `"1.5 MB"`?

**Answer:**

> The underlying value should remain numeric because we need accurate comparisons and calculations. Formatting is a presentation concern and can be done when generating output.

---

# One subtle concept you should remember

We're now moving from:

```text
filesystem
    ↓
discover file
```

to:

```text
filesystem
    ↓
discover file
    ↓
collect metadata
    ↓
FileInfo
```

So our `FileInfo` now represents:

```cpp
struct FileInfo {
    std::filesystem::path path;
    std::uintmax_t size;
};
```

That's becoming the **core representation of a discovered file**.

Later it may grow into:

```text
FileInfo
├── path
├── size
├── hash
└── timestamp
```

But we only add information when the project actually needs it.

---

# Build and test

From:

```text
Reclaim/build/
```

run:

```bash
cmake --build .
```

Then:

```bash
./reclaim.exe
```

Verify that you get:

```text
Files found: N

path | size bytes
```

and that the sizes look reasonable.

---

# Progress

```text
PHASE 1 — DIRECTORY SCANNER

Step 1  Recursive traversal          ✅
Step 2  Files vs directories         ✅
Step 3  Errors & symlinks            ✅
Step 4  FileInfo                     ✅
Step 5  Store FileInfo in vector     ✅
Step 6  File size                    🔄 YOU ARE HERE
Step 7  Command-line path            ⏳
Step 8  Sorting                      ⏳
Step 9  Top-N report                 ⏳
```

**Phase 1:** ~60%
**Overall Reclaim:** ~30%

### Your task

Implement the size handling, build, and run it.

Once you confirm it's working, we'll move to **Step 7: accepting the directory to scan from the command line** — so instead of hardcoding:

```cpp
std::filesystem::path root = ".";
```

we'll be able to do:

```bash
./reclaim.exe C:/Users/Sachi/Documents
```

That's where Reclaim starts behaving like a real CLI tool.
