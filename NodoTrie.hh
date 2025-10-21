#ifndef NODOTRIE_HH
#define NODOTRIE_HH

#include <iostream>
#include <list>
#include <unordered_map>

using namespace std;

class NodoTrie {
   private:
    // La clave del map es el valor del nodo. Por ejemplo, para acceder
    // al nodo con el char 'a' haremos hijos[a].
    unordered_map<char, NodoTrie*> hijos;
    bool hoja;
    list<int> lineas;

   public:
    // Constructor
    NodoTrie();

    NodoTrie(bool h);

    // Destructor
    ~NodoTrie();

    // Insertar nuevo nodo hijo
    void insertar(char c);

    // Insertar nuevo nodo hijo
    void insertar_word(string w, int l);

    // Insertar nuevo nodo hijo
    list<int> buscar_word(string w);

    // Getters y setters
    bool esHoja() const;

    bool contienePalabra(string w);

    NodoTrie* obtener(char c) const;

    NodoTrie* obtener(string w);

    unordered_map<char, NodoTrie*>& getHijos();
};

#endif
