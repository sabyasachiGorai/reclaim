Good. If your **Git initialization, `.gitignore`, staging, and first commit are complete**, then we're ready for the final part of Phase 0.

# Phase 0 — Step 4

## Push Reclaim to GitHub

This is the last major build item in Phase 0. The original plan ends Phase 0 with the project compiled, tracked in Git, and pushed to GitHub. 

Our goal:

```text
Your PC
   │
   │ git push
   ▼
GitHub
   │
   └── Reclaim repository
```

And importantly, GitHub should contain the **source/configuration**, not your generated `build/` directory.

---

# 1. 🧠 LEARN

## What is GitHub?

Git and GitHub are different things.

### Git

Git is the version-control system running locally on your computer.

```text
Your computer
└── Reclaim
    └── .git/
```

### GitHub

GitHub is a remote hosting service where we can store and collaborate on Git repositories.

```text
Your computer                 GitHub
     │                          │
     │      git push            │
     ├─────────────────────────►│
     │                          │
     │      git pull/clone      │
     │◄─────────────────────────┤
```

So:

> **Git manages the repository; GitHub hosts a remote copy of the repository.**

That's an important interview distinction.

---

# 2. What is a remote?

Your local Git repository doesn't automatically know where GitHub is.

We give it a remote address, conventionally called:

```text
origin
```

For example:

```text
local Reclaim
      │
      │ origin
      ▼
GitHub Reclaim repository
```

`origin` is just a conventional name.

It isn't some special GitHub protocol.

You could technically call it:

```text
github
remote1
production
anything
```

but `origin` is the standard convention.

---

# 3. Create the GitHub repository

Go to GitHub and create a **new repository**.

Repository name:

```text
reclaim
```

For this project, I recommend:

```text
Reclaim
```

if GitHub accepts the capitalization as you want.

### Important

When creating the repository, **do not initialize it with**:

* README
* `.gitignore`
* license

because we've already created our local repository and made our first commit.

So initially, make it an **empty repository**.

---

# 4. Add GitHub as the remote

From your local project root:

```text
reclaim/
```

run:

```bash
git remote add origin https://github.com/YOUR_USERNAME/reclaim.git
```

Replace:

```text
YOUR_USERNAME
```

with your GitHub username.

---

# 5. Verify the remote

Run:

```bash
git remote -v
```

You should see something similar to:

```text
origin  https://github.com/YOUR_USERNAME/reclaim.git (fetch)
origin  https://github.com/YOUR_USERNAME/reclaim.git (push)
```

This means your local repository now knows:

```text
origin
   ↓
GitHub repository
```

---

# 6. What is `origin`?

Interview question:

> **What is origin in Git?**

Answer:

> "`origin` is the conventional name given to the default remote repository when working with Git. It usually refers to the remote repository from which the local repository was cloned or to which it was initially connected."

Don't say:

> "`origin` means GitHub."

That's not technically correct.

`origin` is simply a **remote name**.

---

# 7. Check your current branch

Run:

```bash
git branch
```

You may see:

```text
* master
```

or:

```text
* main
```

Our project should use:

```text
main
```

If your current branch is `master`, rename it:

```bash
git branch -M main
```

Then verify:

```bash
git branch
```

You should see:

```text
* main
```

---

# 8. Push the project

Now:

```bash
git push -u origin main
```

The first push uses:

```text
-u
```

which establishes the upstream relationship between your local:

```text
main
```

and remote:

```text
origin/main
```

After this, future pushes can normally be:

```bash
git push
```

instead of:

```bash
git push origin main
```

---

# 9. Understand what actually happens

When you execute:

```bash
git push -u origin main
```

conceptually:

```text
                     LOCAL
                       │
                 Git repository
                       │
                    main
                       │
                       │ git push
                       ▼
                    NETWORK
                       │
                       ▼
                     GitHub
                       │
                  origin/main
```

Git transfers the commits/objects required for the remote repository to have your history.

It's not simply:

> "Upload every file in my folder."

Git is transferring Git repository data.

And because:

```gitignore
build/
```

is in `.gitignore`, your generated build directory isn't part of the commit we're pushing.

---

# 10. Verify on GitHub

Refresh your GitHub repository.

You should see something like:

```text
Reclaim
│
├── .gitignore
├── CMakeLists.txt
└── src/
    └── main.cpp
```

You should **not** see:

```text
build/
reclaim.exe
CMakeCache.txt
CMakeFiles/
```

That's exactly what we want.

---

# 11. 🔥 Interview Questions

## Q1. What's the difference between Git and GitHub?

> **Git is a distributed version-control system used to track changes and maintain repository history locally. GitHub is a platform that hosts Git repositories remotely and provides collaboration features.**

---

## Q2. What is a Git remote?

> **A remote is a reference to another Git repository, usually hosted on a remote server such as GitHub. It allows us to push and pull repository data between the local and remote repositories.**

---

## Q3. What is `origin`?

> **`origin` is the conventional name assigned to the default remote repository. It's just a remote name and isn't inherently tied to GitHub.**

---

## Q4. What does `git push` do?

> **`git push` transfers local commits and the associated Git objects to a remote repository and updates the corresponding remote branch.**

---

## Q5. What does `git pull` do?

> **`git pull` retrieves changes from a remote repository and integrates them into the current local branch. Conceptually, it performs a fetch followed by an integration step, usually merge or rebase depending on configuration or command options.**

---

## Q6. What does `git clone` do?

> **`git clone` creates a new local repository by copying an existing remote repository, including its Git history, and sets up the remote connection.**

---

## Q7. Why did we use `git push -u origin main`?

> **`origin` specifies the remote repository, `main` specifies the branch we're pushing, and `-u` establishes the upstream relationship so that future `git push` and `git pull` commands can use that remote branch by default.**

---

## Q8. Why isn't `build/` on GitHub?

> **Because it's a generated build directory. It contains machine- and configuration-specific artifacts that can be regenerated from the source code and CMake configuration. We exclude it using `.gitignore`.**

---

# 12. A very common interview question

### "If I clone your Reclaim repository, will `reclaim.exe` already be there?"

Your answer:

> **"No. The source repository contains the C++ source code, headers, CMake configuration and other project files, while the build directory and generated executable are ignored. A developer can configure and build the project locally using CMake."**

Later, after Phase 5, our answer changes slightly:

> **"The source repository doesn't contain the build artifacts, but prebuilt binaries are provided separately through GitHub Releases."**

That's exactly the architecture we're aiming for.

---

# 13. Important distinction: repository vs release

Eventually Reclaim will have:

```text
GitHub Repository
│
├── src/
├── include/
├── CMakeLists.txt
├── README.md
└── ...
```

and separately:

```text
GitHub Releases
│
├── reclaim-windows-x64.zip
├── reclaim-linux-x64.tar.gz
└── ...
```

This is much cleaner than committing binaries directly into the source repository.

Our Phase 5 plan specifically calls for building platform-specific binaries and attaching them to a GitHub Release. 

---

# 14. Your commands for this step

Assuming you're at the project root:

```bash
git status
```

Make sure:

```text
nothing to commit, working tree clean
```

Then create the empty GitHub repository.

After that:

```bash
git remote add origin https://github.com/YOUR_USERNAME/reclaim.git
```

Verify:

```bash
git remote -v
```

Then:

```bash
git branch -M main
```

Then:

```bash
git push -u origin main
```

Finally:

```bash
git status
```

---

# ⚠️ Special instruction

Don't create another README, `.gitignore`, or license on GitHub when creating the repository.

We already have our local repository.

We'll create the proper `README.md` later during **Phase 5**, when the actual Reclaim functionality exists. That way the README can document the real application rather than an empty project.

---

# ✅ Step 4 checklist

* [ ] Create empty GitHub repository
* [ ] Add `origin`
* [ ] Verify remote with `git remote -v`
* [ ] Ensure branch is `main`
* [ ] Push with `git push -u origin main`
* [ ] Verify repository on GitHub
* [ ] Confirm `build/` isn't present
* [ ] Confirm `reclaim.exe` isn't present
* [ ] Understand Git vs GitHub
* [ ] Understand remote/origin
* [ ] Understand push
* [ ] Understand why binaries aren't in the source repository

---

# 📊 Progress

### Phase 0

| Step                                             | Status         |
| ------------------------------------------------ | -------------- |
| Step 1 — CMake/project setup                     | ✅              |
| Step 2 — `std::filesystem` + runtime environment | ✅              |
| Step 3 — Git + `.gitignore`                      | ✅*             |
| Step 4 — GitHub + first push                     | 🟡 **Current** |

*Assuming your first commit was successful.

### After Step 4

**Phase 0 → ~90% complete.**

The only remaining Phase 0 work is the **final verification** that the clean repository can be cloned/configured/built successfully. Then Phase 0 is officially finished.

### Entire Reclaim project

**~15–20% of the overall planned work.**

After Phase 0, we finally get to the first real Reclaim functionality:

> **Phase 1 — Directory Scanner**

That's where we'll start learning directory trees, `recursive_directory_iterator`, `FileInfo`, vectors, sorting, command-line arguments, permissions, symlinks, and filesystem errors. 
