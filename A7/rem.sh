#!/bin/bash

# Get the list of shared memory segments
segments=$(ipcs -s | grep "666" | awk '{print $2}')

# Remove each shared memory segment
for s in $segments; do
    ipcrm -s $s
    echo "Removed Semaphore segment $s"
done

echo "All Semaphore segments with permissions 666 have been removed."

