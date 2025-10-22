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
    unordered_map<char, NodoTrie*> hijos;
    bool hoja = false;
    vector<pair<int, int>> lineaPos;
    static int nodos;

    // Convertir carácter a índice
    int charToIndex(char c) const {
        if (c >= 'a' && c <= 'z') return c - 'a';
        if (c >= '0' && c <= '9') return 26 + (c - '0');
        return -1;  // caracter invalido
    }

   public:
    // Constructor
    NodoTrie();

    NodoTrie(bool h);

    // Destructor
    ~NodoTrie();

    // Insertar nuevo nodo hijo
    void insertar(char c);

    // Insertar nuevo nodo hijo
    void insertarPalabra(string w, int l, int p);

    // Insertar nuevo nodo hijo
    vector<pair<int, int>> buscarPalabra(string w);

    // Getters y setters
    bool getHoja() const;

    static int getNodos();

    bool contienePalabra(string w);

    NodoTrie* obtenerNodo(char c) const;

    NodoTrie* obtenerNodo(string w);

    unordered_map<char, NodoTrie*>& getHijos();

    vector<string> completarPrefijo(const string& w);

    void completarDFS(NodoTrie* nodo, string& palabraActual,
                      vector<string>& resultados);

    vector<pair<int, int>> buscarFrase(const vector<string>& frase);

    size_t calcularMemoria() const;
};

#endif
