all: tremolo

tremolo: tremolo.o
		 gcc -o tremolo tremolo.o -lsndfile -lm

tremolo.o: tremolo.c
		   gcc -c tremolo.c 

.PHONY: run
run:
	./tremolo ../data/Voodoo_Child.wav 15000 1200 izlaz.wav


.PHONY: clean
clean:
	rm tremolo.o tremolo izlaz.wav

