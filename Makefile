run : executable
	./executable
compiler : fifo.o main.o
	gcc -o executable fifo.o main.o -lpthread -w
executable: fifo.o main.o
	gcc -o executable fifo.o main.o -lpthread -w
fifo.o: fifo.c
	gcc -o fifo.o -c fifo.c -w
main.o: main.c fifo.h
	gcc -o main.o -c main.c -w
clean : 
	rm main.o fifo.o executable
