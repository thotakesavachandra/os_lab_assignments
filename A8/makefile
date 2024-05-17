all:
	make clean;
	gcc -o master master.c
	gcc -o proc proc.c
	gcc -o sched sched.c
	gcc -o mmu mmu.c

run:
	make all;
	./master 5 5 5

clean:
	rm -f master proc sched mmu result.txt
	bash rem.sh

