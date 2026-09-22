Yes. This is an important distinction to lock in because you'll use these commands throughout Reclaim.

## The simple rule

### If you only change `.cpp` code:

```bash
cmake --build .
```

### If you change `CMakeLists.txt`:

```bash
cmake ..
cmake --build .
```

### If you delete the entire `build/` directory:

You need to configure from scratch:

```bash
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```

The reason we don't keep writing `-G "MinGW Makefiles"` is that **the generator is configuration information stored in the build directory**.

---

# 1. Your current Reclaim setup

We configured the project with:

```bash
cmake -G "MinGW Makefiles" ..
```

That did two things:

```text
1. Read CMakeLists.txt
2. Generate a MinGW Makefiles build system
```

Your `build/` directory now contains the generated configuration, including information about the selected generator.

Conceptually:

```text
reclaim/
│
├── CMakeLists.txt
├── src/
│   └── main.cpp
│
└── build/
    ├── CMakeCache.txt  ← remembers configuration
    ├── CMakeFiles/
    ├── Makefile        ← generated build instructions
    └── ...
```

Therefore, once configured, CMake already knows:

> "This build directory uses the MinGW Makefiles generator."

So:

```bash
cmake --build .
```

knows how to build it.

---

# 2. What happens when you change `main.cpp`?

Suppose you change:

```cpp
std::cout << "Hello";
```

to:

```cpp
std::cout << "Hello Reclaim!";
```

You **do not need to run the generator again**.

Just:

```bash
cmake --build .
```

Why?

Because the build system already knows that:

```text
src/main.cpp
      ↓
needs to be compiled
      ↓
reclaim.exe
```

The build tool checks what has changed and rebuilds what's necessary.

So:

```text
Change C++ source
       ↓
cmake --build .
       ↓
compile changed source
       ↓
link
       ↓
new reclaim.exe
```

---

# 3. What if you change `CMakeLists.txt`?

Now it's different.

Suppose you change:

```cmake
add_executable(reclaim
    src/main.cpp
)
```

to:

```cmake
add_executable(reclaim
    src/main.cpp
    src/scanner.cpp
)
```

You've changed the **build configuration**.

Therefore you need to regenerate the build system:

```bash
cmake ..
```

Then:

```bash
cmake --build .
```

So:

```text
CMakeLists.txt changed
        ↓
     cmake ..
        ↓
regenerate build system
        ↓
cmake --build .
        ↓
compile + link
```

---

# 4. What if you delete `build/`?

This is the important one.

Suppose you do:

```bash
rm -rf build
```

Now you've deleted:

```text
CMakeCache.txt
CMakeFiles/
Makefile
```

and therefore you've deleted the generated build configuration.

CMake no longer has the information that this build directory was configured with:

```text
MinGW Makefiles
```

So you have to configure it again.

From the project root:

```bash
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```

The first command:

```bash
cmake -G "MinGW Makefiles" ..
```

creates the configuration again.

Then:

```bash
cmake --build .
```

builds the project.

---

# 5. Why don't we use `-G` with `cmake --build .`?

Because **`-G` is a configuration/generator option**, not a build option.

This:

```bash
cmake -G "MinGW Makefiles" ..
```

means:

> "Configure this project using the MinGW Makefiles generator."

Whereas:

```bash
cmake --build .
```

means:

> "Build the already-configured project represented by this build directory."

That's why they are separate commands.

Think:

```text
             CONFIGURATION
                  │
                  ▼
cmake -G "MinGW Makefiles" ..
                  │
                  ▼
          build configuration
                  │
                  ▼
                BUILD
                  │
                  ▼
        cmake --build .
```

---

# 6. The useful commands you need to remember

For **our current setup**, these are the important ones.

### First-time configuration

From `reclaim/`:

```bash
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
```

Then:

```bash
cmake --build .
```

---

### Normal development — changed `.cpp`/`.h`

You're inside `build/`:

```bash
cmake --build .
```

That's your most commonly used command.

---

### Changed `CMakeLists.txt`

From `build/`:

```bash
cmake ..
cmake --build .
```

Usually you don't need to specify `-G` again because the existing build directory already has its generator configured.

---

### Deleted `build/`

From project root:

```bash
rm -rf build
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```

---

# 7. What about `cmake -S . -B build`?

There's another style you'll eventually see in professional projects:

```bash
cmake -S . -B build -G "MinGW Makefiles"
```

Instead of:

```bash
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
```

Here:

```text
-S .
```

means:

> source directory is `.`

and:

```text
-B build
```

means:

> build directory is `build`

Then:

```bash
cmake --build build
```

builds it.

So you could eventually use:

```bash
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```

This is cleaner because you can run both commands from the project root.

But **don't switch your workflow just yet**. Understanding the `build/ → cmake .. → cmake --build .` workflow first is useful.

---

# 8. Your interview answer

If they ask:

> **"When do you run `cmake ..` and when do you run `cmake --build .`?"**

Say:

> "`cmake ..` is used to configure or regenerate the build system from the `CMakeLists.txt` in the parent directory. `cmake --build .` actually builds the already-configured project. If I only modify C++ source files, I normally just run `cmake --build .`. If I modify `CMakeLists.txt`, I regenerate the build system with `cmake ..` and then build. If I delete the build directory, I need to configure it again and specify the generator because the previous configuration has been removed."

That's a **complete interview answer**.

---

## One final mental model

Memorize this:

```text
┌──────────────────────────────────────┐
│         CMake CONFIGURATION          │
│                                      │
│ cmake -G "MinGW Makefiles" ..        │
│                                      │
│ Creates/remembers build system       │
└──────────────────┬───────────────────┘
                   │
                   ▼
┌──────────────────────────────────────┐
│             BUILD                    │
│                                      │
│ cmake --build .                      │
│                                      │
│ Uses the existing build system       │
│ to compile/link the project          │
└──────────────────────────────────────┘
```

**`-G` chooses the generator. `--build` builds using the generator that was already configured.**

That distinction is now part of our **Phase 0 interview notes**.
