#ifndef NODOTRIE_HH
#define NODOTRIE_HH

#include <climits>
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

    void calcularProfundidadesAux(int profundidadActual, int& contador,
                                  long long& suma) const;

    int indexLessOcurr(const vector<pair<string, int>>& v);

    int contiene(const vector<pair<string, int>>& v, const string& w);

    vector<string> toVectorString(const vector<pair<string, int>>& v);

   public:
    // Constructor
    NodoTrie();

    NodoTrie(bool h);

    // Destructor
    ~NodoTrie();

    vector<string> completarPrefijo(const string& w, int& nv);

    vector<string> completarPrefijo2(const string& w, int& nv, int n);

    void completarDFS(NodoTrie* nodo, string palabraActual,
                      vector<string>& resultados, int& nv);

    void completarDFS2(NodoTrie* nodo, string palabraActual,
                       vector<pair<string, int>>& resultados, int& nv);

    NodoTrie* obtenerNodo(char c) const;

    NodoTrie* obtenerNodo(string w, int& nv);

    // Insertar nuevo nodo hijo
    void insertar(char c);

    // Insertar nuevo nodo hijo
    void insertarPalabra(string w, int l, int p);

    // Insertar nuevo nodo hijo
    vector<pair<int, int>> buscarPalabra(string w, int& nv);

    vector<pair<int, int>> buscarFrase(const vector<string>& frase, int& nv);

    // Getters y setters
    bool getHoja() const;

    static int getNodos();

    unordered_map<char, NodoTrie*>& getHijos();

    size_t calcularMemoria() const;

    double calcularProfundidadMedia() const;
};

#endif
