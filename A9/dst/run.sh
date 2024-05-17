#!/bin/bash
g++ a.cpp
./a.out < in.txt > out.txt &
PID=$(($!))
echo "PID: $PID"
sleep 2
ps -p $PID -o pid,vsz=MEMORY -o user,group=GROUP -o comm,args=ARGS
diff out.txt ans.txt
