#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// Radix trie (trie comprimido): las aristas pueden etiquetarse con cadenas
// completas, de modo que las secuencias de nodos con un único hijo se agrupan
// en una sola arista. Cada nodo hoja guarda las posiciones (línea, palabra)
// donde aparece la palabra que termina en él.
class NodoRadixTrie {
   private:
    // La clave es la cadena de la arista y el valor, el nodo hijo.
    std::unordered_map<std::string, NodoRadixTrie*> hijos;
    bool hoja = false;
    std::vector<std::pair<int, int>> lineaPos;
    static int nodos;

    // Recorre el subárbol acumulando el número de palabras y la suma de sus
    // profundidades (medidas en aristas) para la profundidad media.
    void calcularProfundidadesAux(int profundidadActual, int& contador,
                                  long long& suma) const;

    // Profundidad máxima medida en aristas hasta una hoja.
    void calcularProfundidadMaximaAux(int profundidadActual,
                                      int& maxProf) const;

    void contarPalabrasAux(int& contador) const;

   public:
    NodoRadixTrie();

    explicit NodoRadixTrie(bool h);

    ~NodoRadixTrie();

    // Longitud del prefijo común más largo entre s1 y s2.
    std::string prefijoCom(const std::string& s1, const std::string& s2);

    // Devuelve todas las palabras que empiezan por el prefijo w.
    std::vector<std::string> contarPalabrasPrefijo(const std::string& w,
                                                   int& nv);

    // Selección por quickselect: devuelve el i-ésimo par mayor por frecuencia.
    std::pair<std::string, int> encontrarIesimoMayor(
        std::vector<std::pair<std::string, int>>& vec, int i);

    int partition(std::vector<std::pair<std::string, int>>& vec, int left,
                  int right);

    // Devuelve las 5 palabras más frecuentes que empiezan por w.
    std::vector<std::string> obtenerTop5MasFrecuentes(const std::string& w,
                                                      int& nv);

    void completarPrefijoAux(const std::string& prefijoBuscado,
                             std::string palabraActual,
                             std::vector<std::string>& resultados, int& nv);

    void completarPrefijoAux2(
        const std::string& prefijoBuscado, std::string palabraActual,
        std::vector<std::pair<std::string, int>>& resultados, int& nv);

    void completarDFS(NodoRadixTrie* nodo, std::string& palabraActual,
                      std::vector<std::string>& resultados, int& nv);

    // Nodo asociado al prefijo w, o nullptr si no existe.
    NodoRadixTrie* obtenerNodo(const std::string& w, int& nv);

    // Inserta una aparición de la palabra w en (línea l, posición p).
    void insertarPalabra(const std::string& w, int l, int p);

    // Devuelve las posiciones donde aparece w, o vacío si no existe.
    std::vector<std::pair<int, int>> buscarPalabra(const std::string& w,
                                                   int& nv);

    // Busca una secuencia de palabras consecutivas dentro de una misma línea.
    std::vector<std::pair<int, int>> buscarFrase(
        const std::vector<std::string>& frase, int& nv);

    // Getters y setters
    bool getHoja() const;

    static int getNodos();

    std::unordered_map<std::string, NodoRadixTrie*>& getHijos();

    std::size_t calcularMemoria() const;

    double calcularProfundidadMedia() const;

    int calcularProfundidadMaxima() const;

    int contarPalabras() const;
};
