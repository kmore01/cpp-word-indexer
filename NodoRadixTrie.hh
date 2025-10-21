#ifndef NODORADIXTRIE_HH
#define NODORADIXTRIE_HH

#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>

using namespace std;

class NodoRadixTrie {
   private:
    // La clave del map es el valor del nodo. Por ejemplo, para acceder
    // al nodo con el char 'a' haremos hijos[a].

   public:
    unordered_map<string, NodoRadixTrie*> hijos;
    bool hoja = false;
    vector<pair<int, int>> lineaPos;
    // Constructor
    NodoRadixTrie();

    NodoRadixTrie(bool h);

    // Destructor
    ~NodoRadixTrie();

    // Insertar nuevo nodo hijo
    void insertar(char c);

    // Insertar nuevo nodo hijo
    void insertar_word(string w, int l, int p);

    // Insertar nuevo nodo hijo
    vector<pair<int, int>> buscar_word(string w);

    // Getters y setters
    bool esHoja() const;

    bool contienePalabra(string w);

    NodoRadixTrie* obtener(char c) const;

    NodoRadixTrie* obtener(string w);

    unordered_map<string, NodoRadixTrie*>& getHijos();

    vector<string> completarPrefijo(const string& w);

    void completarDFS(NodoRadixTrie* nodo, string& palabraActual,
                      vector<string>& resultados);
};

#endif
