Good. Now we move to **Phase 1 — Step 4: Designing `FileInfo`**.

This is the point where Reclaim starts becoming an actual application rather than just a filesystem traversal demo.

# Phase 1 — Step 4: Create `FileInfo`

Right now, our scanner discovers an entry and immediately prints it:

```text
FILE: ./something.txt
FILE: ./folder/photo.jpg
```

But eventually Reclaim needs to answer questions like:

* What is the file's path?
* How large is it?
* What type is it?
* Later: what is its hash?
* Is it a duplicate?
* Should we delete it?

We don't want all of that information scattered across the scanning code.

So we'll introduce a **struct** called `FileInfo`.

---

## 1. What is `FileInfo`?

Think of it as a record describing one file.

For example:

```text
FileInfo
├── path
├── size
└── ...
```

A single file:

```text
C:/Users/Sachi/Documents/test.txt
```

could become:

```text
FileInfo {
    path = "C:/Users/Sachi/Documents/test.txt"
    size = 15240
}
```

Later we can extend it:

```text
FileInfo {
    path
    size
    hash
    modifiedTime
}
```

This is much cleaner than passing around separate variables everywhere.

---

# 2. Create the struct

For this step, we'll create a header file.

### New file

```text
Reclaim/include/FileInfo.h
```

Put this inside:

```cpp
#pragma once

#include <filesystem>
#include <cstdint>

struct FileInfo {
    std::filesystem::path path;
    std::uintmax_t size;
};
```

That's all for now.

Don't add hash, timestamps, etc. yet.

We're deliberately building this incrementally.

---

# 3. Why a header file?

Our project is becoming larger.

Currently:

```text
src/
└── main.cpp
```

We'll eventually have things like:

```text
include/
├── FileInfo.h
├── Scanner.h
├── Hasher.h
└── ...
```

and:

```text
src/
├── main.cpp
├── Scanner.cpp
├── Hasher.cpp
└── ...
```

The header describes the interface/data structure.

The `.cpp` files contain implementation.

For this particular struct, there isn't much implementation, so the entire definition can live in the header.

---

# 4. What is `#pragma once`?

You will see:

```cpp
#pragma once
```

at the top.

It tells the compiler:

> Include this header only once per compilation unit.

Without protection, a header could accidentally be included multiple times and cause duplicate definitions.

You can think of it as:

```text
FileInfo.h
   ↓
included?
   ↓
already included → don't include again
```

It's a common modern C++ technique.

---

# 5. Why `std::filesystem::path`?

We could technically write:

```cpp
std::string path;
```

but we don't want to.

We learned earlier that:

```cpp
std::filesystem::path
```

is specifically designed for filesystem paths.

So:

```cpp
std::filesystem::path path;
```

is semantically more appropriate.

It also gives us filesystem operations directly.

---

# 6. Why `std::uintmax_t` for size?

This one is worth understanding.

Filesystem file sizes can potentially be very large.

C++ filesystem uses an unsigned integer type capable of representing large file sizes.

That's why we use:

```cpp
std::uintmax_t
```

rather than:

```cpp
int
```

or even:

```cpp
long
```

For example:

```cpp
std::uintmax_t size;
```

can represent very large non-negative values.

The standard filesystem API's `file_size()` returns this type as well.

So eventually we'll be able to do:

```cpp
info.size = std::filesystem::file_size(info.path);
```

without unnecessarily converting types.

---

# 7. Now use `FileInfo` in `main.cpp`

Open:

```text
Reclaim/src/main.cpp
```

Add:

```cpp
#include "FileInfo.h"
```

So the file becomes:

```cpp
#include <iostream>
#include <filesystem>
#include <system_error>

#include "FileInfo.h"

int main() {
    std::filesystem::path root = ".";

    std::error_code ec;

    std::filesystem::recursive_directory_iterator it(
        root,
        std::filesystem::directory_options::skip_permission_denied,
        ec
    );

    std::filesystem::recursive_directory_iterator end;

    while (it != end) {

        if (ec) {
            std::cerr << "Error while scanning: "
                      << ec.message()
                      << '\n';

            ec.clear();
        }

        const auto& entry = *it;

        if (entry.is_regular_file(ec)) {

            FileInfo info;
            info.path = entry.path();

            std::cout << "FILE: "
                      << info.path
                      << '\n';
        }
        else if (entry.is_directory(ec)) {
            std::cout << "DIRECTORY: "
                      << entry.path()
                      << '\n';
        }

        it.increment(ec);
    }

    return 0;
}
```

Notice something important.

We aren't doing:

```cpp
FileInfo info;
```

for directories.

Why?

Because **`FileInfo` currently represents a file**, not an arbitrary filesystem entry.

That distinction will become useful later.

---

# 8. What happens now?

The flow is becoming:

```text
filesystem
     ↓
recursive iterator
     ↓
directory_entry
     ↓
is this a regular file?
     ↓
YES
     ↓
create FileInfo
     ↓
store path
```

So instead of:

```text
directory_entry → print
```

we are moving toward:

```text
directory_entry
      ↓
    FileInfo
      ↓
vector<FileInfo>
      ↓
scanner results
```

That last part is coming soon.

---

# 9. Why not just use `directory_entry` everywhere?

This is an excellent interview question.

`directory_entry` is primarily an object provided by the filesystem iterator representing an entry encountered during traversal.

`FileInfo` is **our application's own data model**.

That's an important architectural distinction.

```text
std::filesystem
        ↓
directory_entry
        ↓
      Reclaim
        ↓
     FileInfo
```

The filesystem library shouldn't dictate the entire design of our application.

Our own `FileInfo` lets us decide what information Reclaim actually needs.

Later:

```cpp
struct FileInfo {
    std::filesystem::path path;
    std::uintmax_t size;
    std::string hash;
};
```

Then the duplicate detector doesn't need to know anything about `directory_entry`.

That's good separation of responsibilities.

---

# 10. Build it

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

You should still see the files being scanned.

The important thing isn't the output.

The important thing is that you've successfully introduced your **first application-level data structure**.

---

# Interview Questions

### Q1. Why create `FileInfo` instead of directly using `directory_entry`?

**Answer:**

> `directory_entry` is a filesystem abstraction provided by the standard library, while `FileInfo` is an application-level data model. `FileInfo` allows Reclaim to store only the information it needs and keeps the rest of the application independent from the traversal implementation.

---

### Q2. Why use `std::filesystem::path` instead of `std::string`?

**Answer:**

> `std::filesystem::path` is specifically designed for filesystem paths and provides path manipulation functionality while handling platform-specific path conventions.

---

### Q3. Why use `std::uintmax_t` for file size?

**Answer:**

> Files can be very large, so filesystem file sizes use an unsigned integer type capable of representing a large range. `std::uintmax_t` matches the type returned by `std::filesystem::file_size()`.

---

### Q4. What is the purpose of a struct like `FileInfo`?

**Answer:**

> It groups related information about a file into one logical object. This makes the program easier to maintain and allows other components such as hashing, sorting, and duplicate detection to operate on a consistent representation.

---

### Q5. What's the difference between a library type and an application data model?

**Answer:**

> A library type such as `directory_entry` represents an abstraction provided by the filesystem library. An application data model such as `FileInfo` represents the information required by our own application. Separating them improves modularity and allows the application model to evolve independently.

---

## One thing I want you to notice

We're intentionally **not** doing this yet:

```cpp
std::vector<FileInfo> files;
```

That is the next logical step, but don't jump ahead.

Right now we're learning:

```text
ONE filesystem entry
        ↓
ONE FileInfo object
```

Next we'll learn how to collect:

```text
FileInfo
FileInfo
FileInfo
FileInfo
...
```

into a container so the scanner can return all discovered files.

---

# Progress

```text
PHASE 1 — DIRECTORY SCANNER

Step 1  Recursive traversal          ✅
Step 2  Files vs directories         ✅
Step 3  Errors & symlinks            ✅
Step 4  FileInfo                     🔄 YOU ARE HERE
Step 5  Collect FileInfo objects     ⏳
Step 6  File size                    ⏳
Step 7  Command-line path            ⏳
Step 8  Sorting                      ⏳
Step 9  Top-N report                 ⏳
```

**Phase 1:** ~40%
**Overall Reclaim:** ~25%

### Your task

Create:

```text
include/FileInfo.h
```

modify:

```text
src/main.cpp
```

then:

```bash
cmake --build .
./reclaim.exe
```

Make sure it builds and runs.

When you're done, say **"done"**, and we'll do **Step 5: storing all discovered `FileInfo` objects in a `std::vector`**.
