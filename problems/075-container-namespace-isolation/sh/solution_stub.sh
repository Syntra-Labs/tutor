#!/bin/sh
# solution_stub.sh — Linux Containers from Scratch
# Implement run_isolated() to execute $COMMAND inside $JAIL_DIR with
# isolated PID & mount namespaces and a private /proc.

run_isolated() {
    JAIL_DIR="$1"
    shift
    
    if [ ! -d "$JAIL_DIR" ]; then
        echo "Error: Jail directory '$JAIL_DIR' not found" >&2
        return 1
    fi

    # TODO: Prepare mount points, mount private proc, and chroot into jail
    # Hint: Busybox / Linux provides 'chroot' and mount isolation
    chroot "$JAIL_DIR" "$@"
}
