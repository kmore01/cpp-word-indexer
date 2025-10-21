#ifndef NODOTRIE_HH
#define NODOTRIE_HH

#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>

using namespace std;

class NodoTrie {
   private:
    // La clave del map es el valor del nodo. Por ejemplo, para acceder
    // al nodo con el char 'a' haremos hijos[a].

   public:
    unordered_map<char, NodoTrie*> hijos;
    bool hoja = false;
    vector<pair<int, int>> lineaPos;
    // Constructor
    NodoTrie();

    NodoTrie(bool h);

    // Destructor
    ~NodoTrie();

    // Insertar nuevo nodo hijo
    void insertar(char c);

    // Insertar nuevo nodo hijo
    void insertar_word(string w, int l, int p);

    // Insertar nuevo nodo hijo
    vector<pair<int, int>> buscar_word(string w);

    // Getters y setters
    bool esHoja() const;

    bool contienePalabra(string w);

    NodoTrie* obtener(char c) const;

    NodoTrie* obtener(string w);

    unordered_map<char, NodoTrie*>& getHijos();

    vector<string> completarPrefijo(const string& w);

    void completarDFS(NodoTrie* nodo, string& palabraActual,
                      vector<string>& resultados);

    vector<pair<int, int>> buscarFrase(const vector<string>& frase);
};

#endif
