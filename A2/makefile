all: mgr job
	

mgr: mgr.c
	gcc mgr.c -o mgr

job: job.c
	gcc job.c -o job

clean:
	rm -rf mgr job
	
run: mgr job
	./mgr
