#!/bin/bash

# Get the list of message queues
queues=$(ipcs -q | grep "666" | awk '{print $2}')

# Remove each message queue
for q in $queues; do
    ipcrm -q $q
    echo "Removed message queue $q"
done

echo "All message queues with permissions 666 have been removed."


# Get the list of shared memory segments
segments=$(ipcs -m | grep "666" | awk '{print $2}')

# Remove each shared memory segment
for s in $segments; do
    ipcrm -m $s
    echo "Removed shared memory segment $s"
done

echo "All shared memory segments with permissions 666 have been removed."

