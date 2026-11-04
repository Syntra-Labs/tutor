# Git Internals: Object Database Plumbing

## 1. Content-Addressable DAG Formalism

At its core, Git is not a file version tracker; it is a **content-addressable directed acyclic graph (DAG)** of immutable objects stored under `.git/objects/`.

Every object is identified by the SHA-1 hash of its header and payload:
$$\text{ID}(O) = \text{SHA1}(\text{type} + \text{ " " } + \text{length} + \text{"\0"} + \text{content})$$

The object store contains three core types:
1. **Blob** ($B$): Raw byte content of a file (contains no filename or permissions).
2. **Tree** ($T$): An array of directory entries mapping mode and filename to child tree or blob hashes:
   $$T = [(\text{mode}_i, \text{name}_i, \text{hash}_i)]_{i=1}^k$$
3. **Commit** ($C$): A node in the DAG pointing to a root Tree $T$, zero or more parent Commits $P$, author, committer, and a message:
   $$C = \{T_{\text{root}}, P_1, \dots, P_m, \text{Author}, \text{Committer}, \text{Message}\}$$

High-level porcelain commands (`git add`, `git commit`) wrap low-level **plumbing commands**:
* `git hash-object -w <file>` $\to$ Computes hash and writes blob to `.git/objects/`.
* `git update-index --add --cacheinfo <mode> <hash> <path>` $\to$ Staged index manipulation.
* `git write-tree` $\to$ Serializes index into a Tree object and prints its hash.
* `git commit-tree <tree_hash> -m <msg>` $\to$ Constructs Commit object pointing to tree.
* `git update-ref refs/heads/<branch> <commit_hash>` $\to$ Updates branch ref pointer.

---

## 2. Specification

Implement a POSIX shell script in `solution.sh` that defines:

```sh
create_plumbing_commit() {
    REPO_DIR="$1"
    MESSAGE="$2"
    ...
}
```

### Requirements:
1. Initialize an empty Git repository in `$REPO_DIR` (using `git init "$REPO_DIR"`).
2. Create two files:
   * `README.md` containing `Hello Syntra Labs`
   * `src/main.sh` containing `echo "Syntra Systems"`
3. Store both files in the Git object store using **`git hash-object -w`**.
4. Stage both objects into Git's index using **`git update-index --add --cacheinfo`** (use mode `100644` for files).
5. Generate the tree object using **`git write-tree`**.
6. Create the commit using **`git commit-tree`** with the message passed in `$MESSAGE`.
7. Point `refs/heads/main` to the new commit hash using **`git update-ref`**.
8. **Plumbing Constraint**: You MUST NOT use `git add` or `git commit`! The harness strictly audits command invocations.

---

## 3. Environment & Interactive Testing

Use the **Live Linux VM Terminal** split on the right to inspect `.git/objects/`, cat raw Git objects, and test plumbing commands:

```sh
# View object type and content:
git cat-file -t <hash>
git cat-file -p <hash>

# Verify repository integrity:
git fsck --full
```
