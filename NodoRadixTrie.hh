#ifndef NODORADIXTRIE_HH
#define NODORADIXTRIE_HH

#include <iostream>
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

    void calcularProfundidadesAux(int profundidadActual, int& contador,
                                  long long& suma) const;

    void calcularProfundidadMaximaAux(int profundidadActual,
                                      int& maxProf) const;

    void contarPalabrasAux(int& contador) const;

   public:
    // Constructor
    NodoRadixTrie();

    NodoRadixTrie(bool h);

    // Destructor
    ~NodoRadixTrie();

    // Funcion que te devuelve el prefijo comun entre dos palabras
    string prefijoCom(const string& s1, const string& s2);

    vector<string> completarPrefijo(const string& w, int& nv);

    void completarPrefijoAux(const string& prefijoBuscado, string palabraActual,
                             vector<string>& resultados, int& nv);

    void completarDFS(NodoRadixTrie* nodo, string& palabraActual,
                      vector<string>& resultados, int& nv);

    NodoRadixTrie* obtenerNodo(const string& w, int& nv);

    // Insertar nuevo nodo hijo
    void insertarPalabra(const string& w, int l, int p);

    // Insertar nuevo nodo hijo
    vector<pair<int, int>> buscarPalabra(const string& w, int& nv);

    vector<pair<int, int>> buscarFrase(const vector<string>& frase, int& nv);

    // Getters y setters
    bool getHoja() const;

    static int getNodos();

    unordered_map<string, NodoRadixTrie*>& getHijos();

    size_t calcularMemoria() const;

    double calcularProfundidadMedia() const;

    int calcularProfundidadMaxima() const;

    int contarPalabras() const;
};

#endif