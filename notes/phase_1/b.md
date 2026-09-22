Good. **Phase 1, Step 2 is done.** Now we move to the next concept.

# Phase 1 — Step 3: Handling Filesystem Errors & Symlinks

This is an important step because a real duplicate-file scanner **cannot assume every directory or file can be accessed successfully**.

For example:

```text
C:/
├── Users/
├── Program Files/
├── System Volume Information/   ← may deny access
└── ...
```

If Reclaim crashes whenever it encounters an inaccessible directory, that's not a robust file scanner.

---

## 1. First: understand the problem

Our current code is:

```cpp
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
```

There are operations here that can fail.

For example:

```cpp
entry.is_regular_file()
```

may need filesystem information from the operating system.

And:

```cpp
recursive_directory_iterator(root)
```

may encounter a directory that cannot be opened.

Possible reasons:

* permission denied
* file disappeared while scanning
* broken symbolic link
* filesystem error
* inaccessible network location
* another process changing the directory

A production-quality scanner should **handle these situations instead of simply crashing**.

---

# 2. Exceptions vs `std::error_code`

C++ filesystem APIs generally give us two styles.

### Style 1 — Exceptions

```cpp
try {
    // filesystem operation
}
catch (const std::filesystem::filesystem_error& e) {
    // handle error
}
```

If something goes wrong, an exception is thrown.

---

### Style 2 — `std::error_code`

Instead of throwing, we pass an error-code object:

```cpp
std::error_code ec;
```

and check:

```cpp
if (ec) {
    // error occurred
}
```

For a scanner that may encounter many problematic files/directories, `error_code` can be useful because we can **continue scanning instead of constantly throwing exceptions**.

For now, we're going to learn the `error_code` approach.

---

# 3. Modify our iterator

### File

```text
Reclaim/src/main.cpp
```

Replace the current code with:

```cpp
#include <iostream>
#include <filesystem>
#include <system_error>

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
            std::cout << "FILE: "
                      << entry.path()
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

Don't worry if this looks more complicated than the previous version. **The purpose of this step is to understand why this complexity exists.**

---

# 4. What's new?

There are three important things.

## A. `std::error_code`

```cpp
std::error_code ec;
```

Think of this as a container for an operating-system/filesystem error.

For example:

```text
Permission denied
No such file or directory
I/O error
...
```

Instead of immediately throwing an exception, the filesystem operation can put the error into `ec`.

---

## B. `skip_permission_denied`

```cpp
std::filesystem::directory_options::skip_permission_denied
```

This tells the iterator:

> If you encounter a directory that cannot be accessed because of permissions, skip it rather than failing the traversal.

That's exactly the kind of behavior we want from Reclaim.

Imagine scanning:

```text
C:/
```

and encountering:

```text
C:/System Volume Information
```

We don't want:

```text
Reclaim
   ↓
permission denied
   ↓
💥 program crashes
```

We want:

```text
Reclaim
   ↓
permission denied
   ↓
skip directory
   ↓
continue scanning
```

---

# 5. Why `increment(ec)`?

Previously the iterator automatically advanced:

```cpp
for (...)
```

Now we're controlling it manually:

```cpp
it.increment(ec);
```

This is useful because the increment operation itself can encounter an error.

For example:

```text
directory A
   ↓
directory B
   ↓
directory C ← inaccessible
```

The iterator may encounter an error while trying to move through the tree.

We can capture it in:

```cpp
ec
```

and continue.

---

# 6. Why `std::cerr`?

Notice:

```cpp
std::cerr << "Error while scanning: "
          << ec.message()
          << '\n';
```

We use:

```cpp
std::cerr
```

instead of:

```cpp
std::cout
```

because this is an **error/diagnostic message**.

Generally:

```text
std::cout → normal program output
std::cerr → errors/diagnostics
```

This distinction will become particularly useful later when we design Reclaim's CLI output.

---

# 7. Important: symbolic links

You also need to understand **symbolic links**.

Suppose:

```text
folderA/
├── file.txt
└── linkToFolderB → folderB/

folderB/
└── another.txt
```

A symbolic link isn't necessarily the actual file/directory.

It's more like:

```text
"Go look over there."
```

If a recursive scanner follows symlinks carelessly, you can get situations like:

```text
A
 ↓
symlink → B
 ↓
symlink → A
 ↓
symlink → B
 ↓
...
```

Potentially creating a traversal loop or scanning the same data through multiple paths.

For our **initial Reclaim scanner**, we will use a conservative policy:

> **Do not follow directory symlinks.**

This keeps the scanner predictable and avoids accidentally traversing outside the user's intended directory tree.

We will deal with symlink behavior more explicitly when we build the actual scanner.

---

# 8. One important distinction

Don't confuse:

```cpp
entry.path()
```

with:

```cpp
entry.is_directory()
```

`path` tells us:

> **Where is this filesystem entry?**

while:

```cpp
is_directory()
```

asks:

> **What kind of filesystem entry is this?**

For example:

```text
path:
C:/Users/Sachi/file.txt

type:
regular file
```

---

# 9. Test it

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

You should still see files/directories being printed.

For a controlled test, create something like:

```text
Reclaim/
├── test_data/
│   ├── file1.txt
│   └── folder/
│       └── file2.txt
```

Then temporarily change:

```cpp
std::filesystem::path root = ".";
```

to:

```cpp
std::filesystem::path root = "../test_data";
```

Build and run again.

You should see something similar to:

```text
DIRECTORY: "../test_data/folder"
FILE: "../test_data/folder/file2.txt"
FILE: "../test_data/file1.txt"
```

The exact formatting/order can vary.

---

# Interview Questions

### Q1. Why can filesystem traversal fail?

**Answer:**

> Filesystem operations depend on the operating system and external state, so traversal can fail because of permission restrictions, missing files, broken links, I/O errors, or changes made by other processes while scanning.

---

### Q2. What is `std::error_code`?

**Answer:**

> `std::error_code` represents an error produced by an operation without necessarily throwing an exception. Filesystem APIs provide overloads that accept an error code so the program can inspect and handle errors explicitly.

---

### Q3. Why use `skip_permission_denied`?

**Answer:**

> A file scanner may encounter directories that the current user cannot access. `skip_permission_denied` allows the recursive iterator to skip those directories instead of terminating the traversal.

---

### Q4. Why might Reclaim avoid following symbolic links?

**Answer:**

> Symbolic links can point outside the requested directory tree or create cycles through links. Avoiding directory symlink traversal makes scanning safer and more predictable and prevents repeatedly traversing the same locations.

---

### Q5. Why use `std::cerr`?

**Answer:**

> `std::cerr` is intended for diagnostic and error output, while `std::cout` is normally used for regular program output. Keeping them separate is useful for command-line tools because users can redirect normal output independently from errors.

---

# What you should understand before moving on

Make sure these concepts are clear:

```text
filesystem operation
        ↓
can fail
        ↓
┌──────────────────────┐
│ exception            │
│ OR                   │
│ std::error_code      │
└──────────────────────┘
        ↓
Reclaim handles error
        ↓
continues scanning
```

And:

```text
directory
    ↓
may contain
    ↓
regular files
directories
symlinks
other filesystem entries
```

---

## Step 3 Progress

**Phase 1 — Directory Scanner**

```text
Step 1  Recursive traversal       ✅
Step 2  Files vs directories      ✅
Step 3  Errors & symlinks         🔄 YOU ARE HERE
Step 4  FileInfo                  ⏳
Step 5  File size                 ⏳
Step 6  Command-line path         ⏳
Step 7  Sorting                   ⏳
Step 8  Top-N report              ⏳
```

**Phase 1:** ~25%
**Overall Reclaim:** ~23%

### Your task now

Implement **only this Step 3**, build it, and run it.

Once it works and you understand `error_code`, `skip_permission_denied`, and why we avoid blindly following symlinks, tell me **"done"**.

Then we'll move to **Step 4: designing `FileInfo`**, where Reclaim starts turning raw filesystem entries into useful structured data.
