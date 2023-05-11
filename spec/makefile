all: tremolo

tremolo: tremolo.o
		 gcc -g -o tremolo tremolo.o -lsndfile -lm 

tremolo.o: tremolo.c
		   gcc -c tremolo.c 

.PHONY: run
run:
	./tremolo ../data/np_sample.wav izlaz.wav 4 0.6


.PHONY: clean
clean:
	rm tremolo.o tremolo izlaz.wav

