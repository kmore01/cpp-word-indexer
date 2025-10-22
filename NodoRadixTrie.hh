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
    unordered_map<string, NodoRadixTrie*> hijos;
    bool hoja = false;
    vector<pair<int, int>> lineaPos;
    static int nodos;

   public:
    // Constructor
    NodoRadixTrie();

    NodoRadixTrie(bool h);

    // Destructor
    ~NodoRadixTrie();

    // Funcion que te devuelve el prefijo comun entre dos palabras
    string prefijoCom(string s1, string s2);

    NodoRadixTrie* obtener(string w);

    // Insertar nuevo nodo hijo
    void insertar_word(string w, int l, int p);

    // Insertar nuevo nodo hijo
    vector<pair<int, int>> buscar_word(string w);

    // Getters y setters
    bool esHoja() const;

    static int getNodos();

    unordered_map<string, NodoRadixTrie*>& getHijos();

    vector<pair<int, int>> buscarFrase(const vector<string>& frase);

    vector<string> completarPrefijo(const string& w);

    void completarDFS(NodoRadixTrie* nodo, string& palabraActual,
                      vector<string>& resultados);

    size_t calcularMemoria() const;
};

#endif
