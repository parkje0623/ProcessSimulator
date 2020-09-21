sim: simulation.o LIST.o
	gcc -o sim simulation.o LIST.o
    
simulation.o: simulation.c LIST.c
	gcc -c simulation.c

LIST.o: LIST.c LIST.h
	gcc -c LIST.c
    
clean:
	rm simulation.o sim LIST.o
