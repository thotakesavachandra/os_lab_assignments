run:
	@make init;
	@echo "\n\n"
	@make next_run;


init: 
	gcc rsync.c;
	@echo "--------- CLEARING DST ---------"
	rm -rf dst/*;
	@echo "--------- UNDOING CHANGES ---------"
	sudo bash undo.sh;
	@echo "--------- INITIAL RUN ---------"
	./a.out;


next_run:
	@sudo bash editor.sh;
	@echo "--------- NOW RUNNING SYNC ---------";
	./a.out;
	@echo "--------- SYNC COMPLETE ---------";


clean:
	rm -rf a.out
