Perfect. **Step 1 is officially complete.** You changed the source, rebuilt, and verified that the executable updates correctly. That means you now understand the basic CMake → build tool → compiler → binary pipeline.

And from now on, I'll add a **progress section at the end of every step**, both for the current phase and the entire Reclaim project.

# Phase 0 — Step 2

## `std::filesystem`: How C++ talks to the filesystem

Our Phase 0 checklist specifically says we need to understand `std::filesystem` and why it requires C++17 before moving into the actual directory scanner. 

This step is important because **almost everything Reclaim does later depends on filesystem operations.**

We are **not building the scanner yet**.

We're first learning the foundation.

---

# Part 1 — 🧠 What is `std::filesystem`?

Before C++17, working with files and directories in a portable C++ program was much less convenient.

C++17 introduced:

```cpp
#include <filesystem>
```

and the namespace:

```cpp
std::filesystem
```

It provides standard C++ functionality for interacting with:

* files
* directories
* paths
* file sizes
* timestamps
* directory traversal
* file existence
* copying
* moving
* deleting
* symbolic links

For Reclaim, this is going to be one of our most important standard-library components.

---

# 2. The basic mental model

Think of:

```cpp
std::filesystem
```

as the C++ interface through which our program communicates with the operating system's filesystem.

For example:

```text
                 Reclaim
                    │
                    ▼
           std::filesystem
                    │
                    ▼
             Operating System
                    │
             ┌──────┴──────┐
             ▼             ▼
          Windows        Linux
          NTFS           ext4
          etc.           etc.
```

Our C++ code doesn't need completely different logic for basic filesystem operations on Windows and Linux.

That's one of the major advantages of using the standard library.

---

# 3. `std::filesystem::path`

This is one of the first things you need to understand.

Suppose we have:

```text
D:\Code\Reclaim\src\main.cpp
```

We could represent it as a string:

```cpp
std::string path = "D:\\Code\\Reclaim\\src\\main.cpp";
```

But that's not ideal.

Instead:

```cpp
std::filesystem::path path =
    "D:\\Code\\Reclaim\\src\\main.cpp";
```

A `path` is specifically designed to represent filesystem paths.

That gives us useful operations.

For example:

```cpp
path.filename()
```

could give:

```text
main.cpp
```

and:

```cpp
path.extension()
```

gives:

```text
.cpp
```

and:

```cpp
path.parent_path()
```

gives the parent directory.

---

# 4. Why not just use `std::string`?

This is a good interview question.

### Weak answer

> "Because filesystem has a path class."

That's technically true but doesn't explain anything.

### Better answer

> "`std::filesystem::path` is specifically designed for filesystem paths and provides operations such as extracting filenames, extensions and parent directories. It also handles platform-specific path representations more appropriately than manually manipulating strings."

For example, Windows commonly uses:

```text
C:\Users\...
```

while Unix-like systems use:

```text
/home/user/...
```

We don't want Reclaim's filesystem logic to be full of manual string manipulation for these differences.

---

# 5. `std::filesystem::current_path()`

Let's learn our first actual filesystem function.

```cpp
std::filesystem::current_path()
```

It returns the program's current working directory.

For example, if you execute Reclaim from:

```text
D:\Code\Reclaim\build
```

then:

```cpp
std::filesystem::current_path()
```

represents that directory.

---

# 6. 🔨 BUILD — Modify `main.cpp`

Open:

```text
Reclaim/src/main.cpp
```

Replace its contents with:

```cpp
#include <iostream>
#include <filesystem>

int main() {
    std::filesystem::path currentPath =
        std::filesystem::current_path();

    std::cout << "Reclaim starting...\n";
    std::cout << "Current directory: "
              << currentPath.string()
              << '\n';

    return 0;
}
```

---

# 7. Why did we write it this way?

Let's break it down.

### Include filesystem

```cpp
#include <filesystem>
```

This gives us the filesystem library.

---

### Create a path

```cpp
std::filesystem::path currentPath =
    std::filesystem::current_path();
```

The right side:

```cpp
std::filesystem::current_path()
```

asks for the current working directory.

The result is stored in:

```cpp
std::filesystem::path
```

rather than a plain string.

---

### Convert it to a string

```cpp
currentPath.string()
```

This gives us a string representation suitable for printing.

---

# 8. Build it

From your `build/` directory:

```bash
cmake --build .
```

You **do not need to run**:

```bash
cmake ..
```

again just because you changed `main.cpp`.

This is another practical thing you're learning.

The build system already knows about `main.cpp`.

Run:

```bash
./reclaim.exe
```

You should get something similar to:

```text
Reclaim starting...
Current directory: D:\the Sabyasachi\Code Lab\D\dev\reclaim\build
```

Your exact path will obviously be different.

---

# 9. Important: working directory ≠ executable directory

This is a subtle but **very useful interview concept**.

Suppose:

```text
Reclaim/
├── src/
├── build/
│   └── reclaim.exe
└── ...
```

You execute:

```bash
./reclaim.exe
```

while you're inside:

```text
build/
```

Then:

```cpp
std::filesystem::current_path()
```

will refer to:

```text
build/
```

It does **not necessarily mean**:

> "Where is the executable located?"

It means:

> **"What directory is the process currently being executed from?"**

These are different concepts.

---

# 10. Interview Questions

## Q1. What is `std::filesystem`?

**Answer:**

> "`std::filesystem` is the C++17 standard library facility for working with filesystem entities such as paths, files and directories. It provides portable APIs for operations such as traversal, checking file properties, retrieving file sizes, moving and deleting files."

---

## Q2. Why are we using `std::filesystem` in Reclaim?

**Answer:**

> "Reclaim needs to inspect and manipulate files and directories. `std::filesystem` provides standard C++ APIs for representing paths, traversing directories, retrieving metadata such as file size and timestamps, and eventually moving or deleting files."

---

## Q3. Why does Reclaim require C++17?

**Answer:**

> "`std::filesystem` became part of the C++ standard library in C++17, and Reclaim relies on it for filesystem operations."

---

## Q4. What is `std::filesystem::path`?

**Answer:**

> "`std::filesystem::path` is a type specifically designed to represent filesystem paths. It provides operations for manipulating paths, such as obtaining the filename, extension and parent directory, while handling platform-specific path representations."

---

## Q5. Why not store paths as strings?

**Answer:**

> "A string can represent a path, but it doesn't provide filesystem-specific operations. `std::filesystem::path` gives us type-specific functionality for manipulating and querying paths and provides better portability across operating systems."

---

## Q6. What does `current_path()` return?

**Answer:**

> "`std::filesystem::current_path()` returns the current working directory of the process."

---

## Q7. Is the current working directory necessarily where the executable is located?

**Answer:**

> "No. The current working directory is the directory from which the process is being run, and it can be different from the directory containing the executable."

That's a **very good distinction to know.**

---

# 11. Interview scenario

Interviewer:

> "Suppose Reclaim's executable is in `C:\Reclaim\build`, but I open a terminal in `D:\Downloads` and run the executable using its full path. What will `current_path()` return?"

Answer:

> "It will return `D:\Downloads`, assuming that's the process's working directory. It won't automatically return `C:\Reclaim\build` just because that's where the executable is located."

That's the sort of question that tests whether you actually understand the concept.

---

# 12. ⚠️ Special thing to understand now

Later, when we build:

```text
Reclaim
   ↓
Directory Scanner
```

we'll receive a directory from the user:

```bash
reclaim "D:\Downloads"
```

and we'll convert it into:

```cpp
std::filesystem::path
```

Then we'll use that path to traverse the filesystem.

Eventually:

```text
argv[1]
   │
   ▼
std::filesystem::path
   │
   ▼
directory
   │
   ▼
recursive_directory_iterator
   │
   ▼
FileInfo
   │
   ▼
vector<FileInfo>
```

**But don't implement that yet.**

That's Phase 1.

---

# 13. One thing I want you to notice

Run the program from `build`:

```bash
./reclaim.exe
```

Then move to another directory and run the executable using its path.

For example:

```bash
cd ..
```

and execute it from there.

You'll see that:

```cpp
std::filesystem::current_path()
```

changes.

This is a great little experiment because you'll **see the difference between executable location and working directory yourself**.

---

# ✅ Step 2 completion checklist

Before we mark this step complete:

* [ ] Understand what `std::filesystem` is
* [ ] Understand why C++17 is required
* [ ] Understand `std::filesystem::path`
* [ ] Understand why paths shouldn't simply be treated as strings
* [ ] Understand `current_path()`
* [ ] Understand working directory vs executable location
* [ ] Added `<filesystem>`
* [ ] Used `std::filesystem::path`
* [ ] Used `std::filesystem::current_path()`
* [ ] Rebuilt successfully
* [ ] Ran the executable
* [ ] Changed the working directory and observed the output change
* [ ] Can answer the interview questions without looking

---

# 📊 Reclaim Progress

I'm going to track this at **three levels** from now on.

### Current step

**Phase 0 — Step 2: Filesystem basics**

🟡 **In progress**

### Current phase

**Phase 0 — Environment & Project Setup**

We've completed:

* ✅ Project structure
* ✅ CMake configuration
* ✅ CMake generator/build-tool understanding
* ✅ Successful compilation
* ✅ Successful executable
* 🟡 `std::filesystem`
* ⬜ Git
* ⬜ GitHub

So we're roughly **40% through Phase 0**.

### Entire Reclaim project

We have:

**Phase 0 / 6 phases → ~17% of the planned phases started, but only Phase 0 is being counted as active work.**

More usefully, in terms of the **actual planned checklist**, we're roughly **10–15% through the full project**.

I'll give you an exact-ish progress indicator at the end of every step from now on, rather than pretending every phase has equal size.

**Do Step 2 now. Once your filesystem output works, send it to me. Then we'll move to Step 3: Git + `.gitignore`, where we'll also learn exactly what should and should not go into a C++ repository.**
