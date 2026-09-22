Great. **Step 2 is now complete.** We can move on.

# Phase 0 — Step 3

## Git + `.gitignore` + First Commit

The Phase 0 plan says we need to learn the basic Git workflow:

```text
git init
git add
git commit
.gitignore
```

and then initialize the Reclaim repository and eventually push it to GitHub. 

This step is important because from now on, **every meaningful stage of Reclaim will be version-controlled**.

---

# 🎯 What we're doing in this step

By the end:

```text
Reclaim/
├── .git/
├── .gitignore
├── CMakeLists.txt
├── src/
│   └── main.cpp
├── include/
└── build/
```

And Git should track:

```text
CMakeLists.txt
src/main.cpp
.gitignore
```

but **NOT**:

```text
build/
*.exe
CMakeCache.txt
CMakeFiles/
```

Then we'll make our first commit.

---

# 1. 🧠 What is Git?

Git is a **distributed version-control system**.

Its job is to track changes to your source code over time.

Without Git:

```text
main.cpp
   ↓
change it
   ↓
change it again
   ↓
"Wait... what did I change?"
```

With Git:

```text
Version 1
   ↓
Version 2
   ↓
Version 3
   ↓
Version 4
```

You can inspect what changed, go back to previous versions, create branches, collaborate, etc.

---

# 2. Git does NOT track everything automatically

This is an important concept.

When you run:

```bash
git init
```

Git creates:

```text
.git/
```

inside your project.

That `.git` directory contains Git's repository metadata.

But Git doesn't automatically commit every file.

You explicitly tell Git what you want to put into the next commit.

That's what:

```bash
git add
```

does.

---

# 3. The Git workflow

Understand this model:

```text
                 Working Directory
                        │
                        │ git add
                        ▼
                  Staging Area
                        │
                        │ git commit
                        ▼
                  Git Repository
```

### Working directory

Your actual files:

```text
main.cpp
CMakeLists.txt
...
```

### Staging area

Files you've selected for the next commit.

### Repository

The committed history stored inside:

```text
.git/
```

---

# 4. What does `git init` do?

From the project root:

```text
reclaim/
```

run:

```bash
git init
```

Git creates:

```text
reclaim/
└── .git/
```

You have now turned the directory into a Git repository.

---

# 5. Why do we need `.gitignore`?

This is **very important for C++ projects**.

Our `build/` directory contains generated files:

```text
build/
├── CMakeCache.txt
├── CMakeFiles/
├── Makefile
└── reclaim.exe
```

We **do not want these in Git**.

Why?

Because they're generated from our source/configuration.

Someone cloning the project should be able to generate them themselves.

Our repository should contain:

```text
Source code
+
Build configuration
```

not:

```text
Source code
+
your machine's generated build artifacts
```

---

# 6. Create `.gitignore`

At:

```text
Reclaim/.gitignore
```

create:

```gitignore
# CMake build directory
build/

# Compiled binaries
*.exe
*.dll
*.out

# CMake generated files
CMakeCache.txt
CMakeFiles/
cmake_install.cmake
Makefile

# IDE/editor files
.vscode/
.idea/

# OS-generated files
.DS_Store
Thumbs.db
```

---

# 7. Why are we ignoring DLLs?

Remember the runtime problem we just experienced.

We temporarily copied:

```text
libstdc++-6.dll
libgcc_s_seh-1.dll
```

into `build/`.

We **don't want those accidentally committed**.

Since:

```gitignore
build/
```

already ignores the entire build directory, they're automatically excluded.

---

# 8. Check Git status

From:

```text
reclaim/
```

run:

```bash
git status
```

You should see something roughly like:

```text
Untracked files:
    .gitignore
    CMakeLists.txt
    src/
```

The exact formatting may differ.

Notice that `build/` should **not** appear as an untracked directory because `.gitignore` is excluding it.

---

# 9. Stage the project

Run:

```bash
git add .
```

This means:

> Add all non-ignored changes in the current directory to the staging area.

Then:

```bash
git status
```

You should see something like:

```text
Changes to be committed:

    new file: .gitignore
    new file: CMakeLists.txt
    new file: src/main.cpp
```

---

# 10. Make the first commit

Run:

```bash
git commit -m "Initial Reclaim project setup"
```

This creates your first snapshot.

You now have:

```text
Working directory
       ↓
    git add
       ↓
Staging area
       ↓
   git commit
       ↓
Git history
```

---

# 11. Why shouldn't we commit `build/`?

This is a very common interview question.

### Answer

> "The build directory contains generated artifacts that depend on the local build environment, generator, compiler and configuration. They can be regenerated from the source code and CMake configuration, so committing them would unnecessarily pollute the repository and potentially cause platform-specific conflicts."

That's a strong answer.

---

# 12. Why should `CMakeLists.txt` be committed?

Because it is **source/build configuration**, not a generated artifact.

It describes how the project should be built.

Someone cloning Reclaim should get:

```text
CMakeLists.txt
+
source code
```

and be able to generate:

```text
build/
```

on their own machine.

---

# 13. Interview Questions

### Q1. What is Git?

> Git is a distributed version-control system used to track changes to source code and maintain project history.

---

### Q2. What does `git init` do?

> It initializes a Git repository in the current directory by creating the `.git` directory containing the repository's metadata and history information.

---

### Q3. What does `git add` do?

> It stages changes for the next commit. It moves selected changes from the working directory into the staging area.

---

### Q4. What does `git commit` do?

> It creates a permanent snapshot of the staged changes in the repository's history.

---

### Q5. What's the difference between `git add` and `git commit`?

> `git add` selects changes for the next snapshot by placing them in the staging area, while `git commit` records those staged changes in Git history.

---

### Q6. Why do we use `.gitignore`?

> `.gitignore` specifies files and directories that Git should not track, typically generated files, build artifacts, IDE files, temporary files and other machine-specific data.

---

### Q7. Why are CMake build files usually ignored?

> They're generated from the source configuration and depend on the local build environment. They can be regenerated, so there's generally no reason to store them in the source repository.

---

### Q8. Why don't we commit `reclaim.exe`?

> It's a compiled artifact specific to a particular target platform and build environment. The source and build configuration should be version-controlled, while binaries can be generated separately or distributed through releases.

This becomes particularly important later when we implement **Phase 5 binary distribution**.

---

# 14. A very important distinction for Reclaim

Our GitHub repository will eventually contain:

```text
Reclaim
│
├── source code
├── headers
├── CMakeLists.txt
├── README.md
├── tests
└── GitHub Actions workflows
```

It will **not normally contain**:

```text
build/
reclaim.exe
CMakeCache.txt
CMakeFiles/
```

Instead:

```text
GitHub
   │
   │ clone
   ▼
Source code
   │
   │ cmake
   ▼
Local build
   │
   ▼
Binary
```

And later:

```text
GitHub Actions
       │
       ▼
Build on Windows
       │
       ▼
reclaim.exe
       │
       ▼
GitHub Release
```

That's the architecture we're ultimately heading toward.

---

# 15. Special note: your temporary DLLs

Right now, if you haven't already deleted them, make sure the temporary files:

```text
libstdc++-6.dll
libgcc_s_seh-1.dll
```

are gone from `build/`.

You already ran:

```bash
rm libstdc++-6.dll libgcc_s_seh-1.dll
```

so you should be fine.

And because:

```gitignore
build/
```

is ignored, even if they were still there, Git wouldn't stage them.

---

# 16. Build → Git relationship

You should now understand the relationship:

```text
             SOURCE
                │
       ┌────────┴────────┐
       │                 │
 CMakeLists.txt       src/
       │                 │
       └────────┬────────┘
                │
              Git
                │
                ▼
          GitHub repository
                │
                ▼
             clone
                │
                ▼
              CMake
                │
                ▼
             build/
                │
                ▼
          reclaim.exe
```

Git stores the **inputs** needed to recreate the build, not every generated output.

---

# 17. Your commands for this step

From the **project root**, not `build/`:

```bash
cd ..
```

if you're currently inside `build/`.

Then:

```bash
git init
```

Create `.gitignore`.

Then:

```bash
git status
```

Then:

```bash
git add .
```

Then:

```bash
git status
```

Verify `build/` isn't staged.

Then:

```bash
git commit -m "Initial Reclaim project setup"
```

Finally:

```bash
git status
```

You ideally want:

```text
nothing to commit, working tree clean
```

---

# 🎯 Interview mini-test

Before you consider this step complete, answer these **without looking above**:

1. What does `git init` do?
2. What is the staging area?
3. What does `git add` do?
4. What does `git commit` do?
5. Why don't we commit `build/`?
6. Why do we commit `CMakeLists.txt`?
7. What is the purpose of `.gitignore`?
8. What's the difference between source files and build artifacts?

If you can explain those naturally, **Step 3 is done.**

---

# 📊 Progress

### Phase 0

| Step                                     | Status         |
| ---------------------------------------- | -------------- |
| Step 1 — CMake/project setup             | ✅ Complete     |
| Step 2 — `std::filesystem` + environment | ✅ Complete     |
| Step 3 — Git + `.gitignore`              | 🟡 **Current** |

**Phase 0: ~50% → ~65% after this step**

Remaining Phase 0:

* GitHub repository
* Push project
* Final Phase 0 verification

### Entire Reclaim

**~15% complete**

And importantly, we're still building the foundation before touching the actual duplicate detection logic. That is deliberate.
