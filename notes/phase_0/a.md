Absolutely. From this point onward, **every step of Reclaim will follow the same structure** so you can build it and simultaneously prepare to defend it in an interview.

We are starting **Phase 0 — Environment & Project Setup**. The existing plan says Phase 0 covers CMake, the build workflow, `std::filesystem`, Git, project structure, and producing the first working binary. 

# Phase 0 — Step 1

## Create the project structure + understand CMake

### Our goal for this step

By the end of this step, you should have:

```text
Reclaim/
├── CMakeLists.txt
├── src/
│   └── main.cpp
├── include/
├── build/
└── .gitignore
```

And you should understand:

* What a C++ project actually consists of
* What `CMakeLists.txt` does
* What `cmake ..` actually does
* What `cmake --build .` actually does
* Difference between source code, build files, object files, and executable
* Why we keep `build/` separate
* What `CMAKE_CXX_STANDARD 17` means
* Why we're using C++17
* How the compiler fits into this process

---

# 1. 🧠 CONCEPTS YOU NEED TO LEARN

## 1.1 What are we actually building?

Reclaim is ultimately an **executable program**.

Our source code might look like:

```text
main.cpp
scanner.cpp
hash.cpp
...
```

But the computer doesn't directly execute `.cpp` files.

The general process is:

```text
C++ source code
      ↓
   Compiler
      ↓
 Object files
      ↓
    Linker
      ↓
 Executable binary
```

For example:

```text
main.cpp
   ↓
g++
   ↓
main.o
   ↓
linker
   ↓
reclaim.exe
```

Later, when our project becomes larger:

```text
main.cpp
scanner.cpp
hash.cpp
report.cpp
      ↓
    compiler
      ↓
main.o scanner.o hash.o report.o
      ↓
     linker
      ↓
  reclaim.exe
```

This distinction is **very important for interviews**.

---

# 2. Why are we using CMake?

You could compile a tiny program manually:

```bash
g++ main.cpp -o reclaim
```

But imagine having:

```text
20 .cpp files
10 header files
external libraries
different operating systems
different compilers
debug/release builds
compiler flags
tests
```

Manually managing that becomes painful.

CMake lets us describe the project:

> "Here are my source files, here are my requirements, and here is what I want to build."

CMake then generates the appropriate build system for the platform.

So conceptually:

```text
CMakeLists.txt
      ↓
     CMake
      ↓
Build system
      ↓
Compiler + Linker
      ↓
Executable
```

### Important distinction

**CMake is not the compiler.**

CMake doesn't compile your C++ code itself.

It **configures/generates the build system** that will invoke the compiler.

This is one of the most common interview questions.

---

# 3. `cmake ..` vs `cmake --build .`

This distinction is important enough that you should memorize the mental model.

Suppose we're inside:

```text
Reclaim/build/
```

and run:

```bash
cmake ..
```

The `..` means:

> Go to the parent directory.

So CMake looks at:

```text
Reclaim/CMakeLists.txt
```

and configures the project.

Conceptually:

```text
build/
   ↓
cmake ..
   ↓
read ../CMakeLists.txt
   ↓
configure project
   ↓
generate build files
```

It **doesn't mean "compile my program."**

Then:

```bash
cmake --build .
```

means:

> Build the project using the generated build system in the current directory.

So:

```text
cmake ..
     ↓
CONFIGURE / GENERATE

cmake --build .
     ↓
BUILD / COMPILE / LINK
```

### Interview answer

**Q: What is the difference between `cmake ..` and `cmake --build .`?**

**Answer:**

> "`cmake ..` configures the project using the `CMakeLists.txt` in the parent directory and generates the native build files. `cmake --build .` then uses those generated build files to compile and link the project."

That's an answer you should be able to say naturally.

---

# 4. Why have a separate `build/` directory?

We don't want generated files mixed with our source code.

Bad:

```text
Reclaim/
├── main.cpp
├── CMakeFiles/
├── Makefile
├── *.o
├── reclaim.exe
└── ...
```

Instead:

```text
Reclaim/
├── CMakeLists.txt
├── src/
│   └── main.cpp
├── include/
└── build/
    ├── CMakeFiles/
    ├── ...
    └── reclaim.exe
```

The source tree stays clean.

This is called an **out-of-source build**.

It also makes it easy to completely remove the generated build artifacts:

```bash
rm -rf build
```

and configure from scratch.

On Windows, you can simply delete the `build` folder.

---

# 5. Why C++17?

Our project will use:

```cpp
#include <filesystem>
```

`std::filesystem` became part of the C++ standard library in **C++17**.

And Reclaim is fundamentally a filesystem application.

We'll eventually use things like:

```cpp
std::filesystem::path
std::filesystem::directory_iterator
std::filesystem::recursive_directory_iterator
std::filesystem::file_size
std::filesystem::remove
std::filesystem::rename
```

So C++17 is a natural minimum standard for this project.

---

# 6. 🔨 BUILD — Step 1

Now let's actually create the project.

## Step 1.1 — Create the root directory

Choose wherever you keep your coding projects.

For example:

```text
D:\Projects\Reclaim
```

Your exact location doesn't matter.

Inside it create:

```text
Reclaim/
```

---

## Step 1.2 — Create the folders

Inside `Reclaim`:

```text
Reclaim/
├── src/
├── include/
└── build/
```

**Important:** technically, you don't need to manually create `build/`.

CMake can generate it after you create it yourself, but for learning the workflow we'll create it explicitly.

---

# Step 1.3 — Create `CMakeLists.txt`

Create this file:

```text
Reclaim/CMakeLists.txt
```

Put:

```cmake
cmake_minimum_required(VERSION 3.20)

project(Reclaim VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(reclaim
    src/main.cpp
)

target_include_directories(reclaim PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)
```

Don't just copy this.

We will understand every line.

---

# 7. Understanding `CMakeLists.txt`

### This:

```cmake
cmake_minimum_required(VERSION 3.20)
```

tells CMake:

> This project requires at least CMake 3.20.

---

### This:

```cmake
project(Reclaim VERSION 1.0 LANGUAGES CXX)
```

defines our project.

The project name is:

```text
Reclaim
```

and we're telling CMake this is a C++ project:

```text
CXX
```

---

### This:

```cmake
set(CMAKE_CXX_STANDARD 17)
```

means:

> Compile this project using C++17.

---

### This:

```cmake
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

means:

> Don't silently fall back to an older C++ standard.

That's useful because our code expects C++17.

---

### This:

```cmake
set(CMAKE_CXX_EXTENSIONS OFF)
```

asks for standard C++ rather than compiler-specific language extensions.

This improves portability.

---

### This:

```cmake
add_executable(reclaim
    src/main.cpp
)
```

is particularly important.

We're telling CMake:

> Create an executable target called `reclaim` using `src/main.cpp`.

So:

```text
reclaim
```

is our executable target name.

On Windows you'll eventually see something like:

```text
reclaim.exe
```

---

### Finally:

```cmake
target_include_directories(reclaim PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)
```

This tells the compiler where our project's header files will live.

We're not using any custom headers yet, but we're establishing the structure now.

Later we'll have things like:

```text
include/
├── Scanner.h
├── Hasher.h
└── FileInfo.h
```

---

# 8. Create `main.cpp`

Create:

```text
Reclaim/src/main.cpp
```

For this step:

```cpp
#include <iostream>

int main() {
    std::cout << "Reclaim starting...\n";
    return 0;
}
```

That's deliberately simple.

We're not implementing the scanner yet.

---

# 9. Your complete structure

You should now have:

```text
Reclaim/
│
├── CMakeLists.txt
│
├── src/
│   └── main.cpp
│
├── include/
│
└── build/
```

Don't worry if `build/` becomes full of files after the next step.

That's expected.

---

# 10. Build it

Open your terminal in:

```text
Reclaim/
```

Then:

```bash
cd build
```

You should now be here:

```text
Reclaim/build/
```

Run:

```bash
cmake ..
```

Then:

```bash
cmake --build .
```

If everything is configured correctly, CMake should compile the project.

---

# 11. Run the binary

On Windows, the exact location can depend on the generator CMake selected.

If you're using the setup we've been working with, first inspect:

```text
build/
```

and locate:

```text
reclaim.exe
```

Then execute it.

For example, depending on the generated build structure:

```bash
./reclaim.exe
```

or:

```bash
./Debug/reclaim.exe
```

You should see:

```text
Reclaim starting...
```

---

# 12. 🧪 TEST

Don't move on yet.

We want to deliberately understand the build process.

### Test 1

Change:

```cpp
Reclaim starting...
```

to:

```cpp
Reclaim starting successfully!
```

Then run:

```bash
cmake --build .
```

Notice that you **don't normally need to run `cmake ..` again** just because you changed the contents of `main.cpp`.

Why?

Because the build system already knows about `main.cpp`.

---

### Test 2

Now change `CMakeLists.txt`.

For example:

```cmake
project(Reclaim VERSION 1.1 LANGUAGES CXX)
```

Then run:

```bash
cmake ..
```

and:

```bash
cmake --build .
```

This demonstrates the difference between **configuration** and **building**.

---

# 13. 🎯 INTERVIEW PREPARATION

These are your Phase 0 Step 1 interview questions.

Don't just read the answers. Try answering them yourself first.

---

### Q1. What is CMake?

**Answer:**

> CMake is a cross-platform build-system generator. We describe the project's build requirements in `CMakeLists.txt`, and CMake generates the appropriate native build files for the platform. Those build files are then used to compile and link the C++ project.

---

### Q2. Is CMake a compiler?

**Answer:**

> No. CMake is a build-system generator. It configures the project and generates build files that invoke a compiler and linker.

---

### Q3. What happens when you run `cmake ..`?

**Answer:**

> CMake reads the `CMakeLists.txt` from the parent directory, configures the project, detects the available compiler and build environment, and generates the build system inside the current build directory.

---

### Q4. What happens when you run `cmake --build .`?

**Answer:**

> It invokes the build system that CMake previously generated in the current directory. That build system compiles the source files and links the resulting object files into the executable.

---

### Q5. Why do we use a separate build directory?

**Answer:**

> To keep generated build artifacts separate from source code. This is called an out-of-source build. It keeps the repository clean and allows us to delete and regenerate the entire build directory without affecting the source.

---

### Q6. Why are we using C++17?

**Answer:**

> Reclaim is a filesystem-oriented application, and `std::filesystem` became part of the standard C++ library in C++17. Using C++17 gives us the standard filesystem APIs we need while maintaining good compiler and platform support.

---

### Q7. What is the difference between source code and a binary?

**Answer:**

> Source code is human-readable C++ code that must be compiled. A binary is the compiled executable containing machine code and other information required by the operating system to execute the program.

---

### Q8. What does the compiler do?

**Answer:**

> The compiler translates C++ source code into lower-level object code while performing tasks such as parsing, type checking and optimization.

---

### Q9. What does the linker do?

**Answer:**

> The linker combines object files and resolves references between them and required libraries to produce the final executable.

---

### Q10. Why can't we simply execute `main.cpp`?

**Answer:**

> The operating system executes machine-code binaries, not C++ source files. The C++ source must first be compiled and linked into an executable.

---

### Q11. What does `add_executable()` do in CMake?

**Answer:**

> It defines an executable target and specifies the source files needed to build that executable.

---

### Q12. Why did we specify `LANGUAGES CXX`?

**Answer:**

> `CXX` is CMake's identifier for the C++ language. It tells CMake that this project requires a C++ compiler and enables the corresponding C++ configuration.

---

# 14. 🔥 A question specifically relevant to your compiler setup

You previously configured GCC through MSYS2/MinGW-w64.

An interviewer could ask:

> **"What compiler are you using?"**

You should be able to explain:

> "I'm using GCC through the MinGW-w64 toolchain on Windows. CMake detects the compiler and generates a build system around it. The C++ compiler is responsible for compiling the source, while the linker produces the final executable."

And remember:

```text
CMake
  ↓
Build system
  ↓
GCC / G++
  ↓
Object files
  ↓
Linker
  ↓
reclaim.exe
```

CMake is **not replacing GCC**.

---

# 15. ⚠️ Special note about cross-platform binaries

This connects directly to something you asked earlier.

If we compile:

```text
Reclaim
   ↓
Windows + MinGW-w64
   ↓
reclaim.exe
```

that binary is a **Windows binary**.

We cannot simply rename it:

```text
reclaim.exe → reclaim
```

and expect it to become a macOS executable.

The binary is built for a particular target platform/architecture/toolchain/ABI.

So eventually:

```text
Windows build
    ↓
Windows binary

Linux build
    ↓
Linux binary

macOS build
    ↓
macOS binary
```

This is exactly why our eventual GitHub Actions distribution phase will build platform-specific binaries rather than trying to ship one universal executable. The project plan already calls for automated Linux and Windows builds and attaching the resulting binaries to a GitHub Release. 

---

# 16. 📁 Naming convention we're going to follow

We'll keep this consistent throughout Reclaim.

```text
Reclaim/
│
├── CMakeLists.txt
│
├── README.md
│
├── .gitignore
│
├── src/
│   ├── main.cpp
│   ├── scanner.cpp
│   ├── hasher.cpp
│   └── ...
│
├── include/
│   ├── scanner.h
│   ├── hasher.h
│   └── ...
│
├── tests/
│   └── ...
│
└── build/
    └── generated files
```

We'll make the structure evolve as the project grows rather than prematurely creating 30 empty files.

---

# 17. 📝 Your Phase 0 notes

For your personal notes, remember this mental model:

```text
                CMakeLists.txt
                      │
                      ▼
                    CMake
                      │
              configure/generate
                      │
                      ▼
               Build system
                      │
                      ▼
               Compiler (G++)
                      │
                      ▼
                Object files
                      │
                      ▼
                  Linker
                      │
                      ▼
                Reclaim binary
```

And the two commands:

```text
cmake ..
     =
configure + generate
```

```text
cmake --build .
     =
compile + link
```

That's the core of **Step 1**.

---

## ✅ Step 1 completion checklist

Before we call this step finished, verify that you personally have:

* [ ] `Reclaim/` created
* [ ] `CMakeLists.txt` created
* [ ] `src/main.cpp` created
* [ ] `include/` created
* [ ] `build/` created
* [ ] `cmake ..` works
* [ ] `cmake --build .` works
* [ ] `reclaim` binary is produced
* [ ] Binary executes successfully
* [ ] You understand CMake ≠ compiler
* [ ] You understand configure vs build
* [ ] You understand compiler vs linker
* [ ] You understand source code vs binary
* [ ] You can answer the interview questions above without simply memorizing the wording

**Do not start Step 2 yet.**

Get this compiling on your machine first. Once it works, show me your **project structure and the output of `cmake ..` / `cmake --build .`**, and we'll move to the next step.
