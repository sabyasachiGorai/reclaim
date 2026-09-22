Perfect. **Step 4 is done.** Now we make the scanner actually collect the files it finds.

# Phase 1 — Step 5: Store `FileInfo` Objects in a `std::vector`

So far, our scanner does this:

```text
filesystem entry
       ↓
is it a file?
       ↓
create FileInfo
       ↓
print it
```

But once `main()` finishes, that `FileInfo` disappears.

That's not useful for Reclaim.

Eventually we need:

```text
Scanner
   ↓
FileInfo
FileInfo
FileInfo
FileInfo
   ↓
collection
   ↓
duplicate detection
```

The natural C++ container for this is:

```cpp
std::vector<FileInfo>
```

---

## 1. Why `std::vector`?

A vector is a **dynamic contiguous array**.

For example:

```cpp
std::vector<FileInfo> files;
```

starts empty:

```text
files
[]
```

Then:

```cpp
files.push_back(file1);
```

becomes:

```text
[file1]
```

Then:

```cpp
files.push_back(file2);
```

becomes:

```text
[file1, file2]
```

And so on.

This is exactly what we need because we don't know beforehand how many files the user will have.

---

# 2. Modify `main.cpp`

### File

```text
Reclaim/src/main.cpp
```

Add:

```cpp
#include <vector>
```

Then create the vector before the traversal:

```cpp
std::vector<FileInfo> files;
```

Your current `main.cpp` should now look like this:

```cpp
#include <iostream>
#include <filesystem>
#include <system_error>
#include <vector>

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

    std::vector<FileInfo> files;

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

            files.push_back(info);
        }
        else if (entry.is_directory(ec)) {
            std::cout << "DIRECTORY: "
                      << entry.path()
                      << '\n';
        }

        it.increment(ec);
    }

    std::cout << "\nFiles found: "
              << files.size()
              << '\n';

    return 0;
}
```

Notice that we **stopped printing every file immediately**.

Instead:

```cpp
files.push_back(info);
```

stores it.

Then after scanning:

```cpp
files.size()
```

tells us how many files we found.

---

# 3. What exactly does `push_back()` do?

This:

```cpp
files.push_back(info);
```

adds `info` to the end of the vector.

Conceptually:

```text
Before:

files
┌────────┬────────┐
│ file A │ file B │
└────────┴────────┘


push_back(file C)


After:

files
┌────────┬────────┬────────┐
│ file A │ file B │ file C │
└────────┴────────┴────────┘
```

The vector manages its own memory.

You don't need:

```cpp
new
delete
malloc
free
```

This is one of the major advantages of using C++ containers.

---

# 4. Why not use a normal array?

You could technically do:

```cpp
FileInfo files[1000];
```

but that's a poor fit.

You would have to decide:

> How many files can Reclaim scan?

1000?

10,000?

1 million?

You don't know.

A vector grows dynamically.

For example, conceptually:

```text
capacity = 4

[file1][file2][file3][file4]
```

If another element is added and capacity isn't enough, the vector allocates a larger block and moves/copies the elements.

You don't manually manage that process.

---

# 5. `size()` vs `capacity()`

This is an important interview concept.

Suppose:

```cpp
std::vector<FileInfo> files;
```

contains 5 elements.

Then:

```cpp
files.size()
```

means:

> How many actual elements are currently stored?

While:

```cpp
files.capacity()
```

means roughly:

> How many elements can currently fit in the allocated storage before another allocation is needed?

For example:

```text
size     = 5
capacity = 8
```

means:

```text
5 actual objects
3 unused slots available
```

Don't worry about manually controlling capacity yet.

---

# 6. Why `std::vector` is useful later

This is where our architecture starts becoming interesting.

Once we have:

```cpp
std::vector<FileInfo> files;
```

we can do things like:

```text
scan
 ↓
vector<FileInfo>
 ↓
sort by size
 ↓
find candidates
 ↓
hash candidates
 ↓
group duplicates
```

So `vector` becomes the bridge between different stages of Reclaim.

---

# 7. One important memory concept

A `FileInfo` currently contains:

```cpp
struct FileInfo {
    std::filesystem::path path;
    std::uintmax_t size;
};
```

When you do:

```cpp
files.push_back(info);
```

the `FileInfo` object is stored **inside the vector**.

The local:

```cpp
FileInfo info;
```

is temporary.

Conceptually:

```text
while loop
    ↓
create info
    ↓
copy/move into vector
    ↓
next iteration
    ↓
old local info disappears
    ↓
vector's copy remains
```

This is an important difference between:

```text
local variable
```

and:

```text
object stored inside a container
```

Later we'll discuss move semantics and `emplace_back`, but **don't worry about optimizing this yet**.

---

# 8. Build and run

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

At the end you should see something like:

```text
DIRECTORY: "./src"
DIRECTORY: "./include"
...
Files found: 7
```

Your number will obviously depend on what's currently inside your project.

---

# 9. Interview questions

### Q1. Why use `std::vector` to store discovered files?

**Answer:**

> The number of files is not known beforehand, so a dynamically sized container is appropriate. `std::vector` provides contiguous storage, efficient iteration, and automatic memory management.

---

### Q2. What is the difference between `size()` and `capacity()`?

**Answer:**

> `size()` is the number of elements currently stored in the vector. `capacity()` is the amount of storage currently allocated for elements before another allocation may be required.

---

### Q3. What happens when a vector runs out of capacity?

**Answer:**

> The vector allocates a larger block of memory, moves or copies its existing elements into the new storage, and releases the old storage. This is why individual `push_back` operations are amortized constant time.

---

### Q4. Why is vector memory management safer than manually allocating an array?

**Answer:**

> `std::vector` manages its memory automatically using RAII. It grows when necessary and releases its memory automatically when it is destroyed, reducing the risk of leaks and invalid memory management.

---

### Q5. What is the time complexity of `push_back()`?

**Answer:**

> Normally it is O(1) amortized. Most insertions don't require reallocation, but occasionally the vector must allocate new storage and move its elements, making that particular insertion O(n).

That **"amortized O(1)"** phrase is worth remembering for interviews.

---

# One design improvement to notice

Our `main.cpp` is starting to become crowded:

```text
main.cpp
├── traversal
├── error handling
├── FileInfo creation
├── vector management
└── output
```

That's intentional for now.

We're learning the concepts first.

Soon we'll extract this logic into a dedicated **Scanner** component:

```text
main.cpp
    ↓
Scanner
    ↓
vector<FileInfo>
```

That will be a much cleaner design.

---

# Progress

```text
PHASE 1 — DIRECTORY SCANNER

Step 1  Recursive traversal          ✅
Step 2  Files vs directories         ✅
Step 3  Errors & symlinks            ✅
Step 4  FileInfo                     ✅
Step 5  Store FileInfo in vector     🔄 YOU ARE HERE
Step 6  File size                    ⏳
Step 7  Command-line path            ⏳
Step 8  Sorting                      ⏳
Step 9  Top-N report                 ⏳
```

**Phase 1:** ~50%
**Overall Reclaim:** ~28%

### Your task

Implement this step, build, and run:

```bash
cmake --build .
./reclaim.exe
```

You should get a final:

```text
Files found: N
```

Once that works, say **done**.

Then we'll tackle **Step 6: getting the actual file size**, which is the first piece of information that will become important for finding duplicate-file candidates efficiently.
