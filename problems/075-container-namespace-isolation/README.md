# Linux Containers from Scratch: Process & Mount Namespaces

## 1. Mathematical & Systems Formalism

Modern Linux containers (Docker, containerd, runc) are not hardware virtual machines. A container is a standard Linux process governed by two fundamental kernel primitives:

1. **Namespaces** ($\mathcal{N}$): Partitioning of global system resources. When a process belongs to a namespace, changes to that resource are only visible to members of that namespace:
   $$\mathcal{N}_{\text{pid}} \implies \text{Isolated PID tree where the child is PID } 1$$
   $$\mathcal{N}_{\text{mnt}} \implies \text{Isolated virtual file system mount table}$$
   $$\mathcal{N}_{\text{uts}} \implies \text{Isolated hostname/domain identification}$$

2. **Root Filesystem Pivot / Chroot** ($\mathcal{R}$):
   $$\mathcal{R}: \text{Path} \to \text{Jail Root } \Omega$$
   Restricts pathname resolution so that path traversal starting with `/` never ascends beyond directory $\Omega$.

Inside a properly isolated container:
* The root process must observe itself as **PID 1** (`/proc/1/cmdline`).
* Host processes must be invisible from inside the container `/proc` tree.
* Filesystem paths outside the jail boundary must be inaccessible.

---

## 2. Specification

Implement a POSIX shell script in `solution.sh` defining the function:

```sh
run_isolated() {
    JAIL_DIR="$1"
    shift
    COMMAND="$@"
    ...
}
```

### Requirements:
1. **Directory Preparation**: Ensure `$JAIL_DIR/proc`, `$JAIL_DIR/bin`, `$JAIL_DIR/lib`, etc., exist or are mountable.
2. **Mount Namespace**: Mount a private instance of `proc` inside `$JAIL_DIR/proc` so that processes inside the container read an isolated `/proc` without polluting the host's `/proc`.
3. **PID Namespace**: Execute the target command inside a new PID namespace where the child process runs as PID 1 (`$$ == 1`).
4. **Filesystem Confinement**: Ensure the process cannot break out of `$JAIL_DIR` (using `chroot`).
5. **Exit Code Propagation**: Propagate the exact exit code of `$COMMAND` back to the caller.

---

## 3. Environment & Testing

You have access to a real, live Linux kernel running inside your browser via the **Live Linux VM Terminal** split on the right. You can test commands interactively in the VM:

```sh
# Inspect namespaces in current shell:
ls -la /proc/$$/ns

# Test chroot and private proc mounts:
chroot /my_jail /bin/sh
```
