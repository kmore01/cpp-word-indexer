all: main

main: main.o NodoTrie.o NodoRadixTrie.o
	g++ -o main *.o

main.o: main.cc
	g++ -c main.cc -O2

NodoTrie.o: NodoTrie.cc NodoTrie.hh
	g++ -c NodoTrie.cc -O2

NodoRadixTrie.o: NodoRadixTrie.cc NodoRadixTrie.hh
	g++ -c NodoRadixTrie.cc -O2

clean:
	rm -f *.o
	rm -f main