#ifndef NODOTRIE_HH
#define NODOTRIE_HH

#include <iostream>
#include <unordered_map>

using namespace std;

class NodoTrie {
   private:
    // La clave del map es el valor del nodo. Por ejemplo, para acceder
    // al nodo con el char 'a' haremos hijos[a].
    unordered_map<char, NodoTrie *> hijos;
    bool esHoja;

   public:
    // Constructor
    NodoTrie();

    // Destructor
    ~NodoTrie();

    // Getters y setters
    bool esHoja() const;

    void setEsHoja(bool eh);

    unordered_map<char, NodoTrie *> &getHijos();

    // Métodos
    void mostrarDatos() const;
};

#endif
