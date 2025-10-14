#ifndef NODOTRIE_HH
#define NODOTRIE_HH

#include <iostream>

using namespace std;

class NodoTrie {
   private:
    char valor;
    bool esHoja;

   public:
    // Constructor
    NodoTrie(string n, int e);

    // Métodos
    void mostrarDatos() const;
};

#endif
