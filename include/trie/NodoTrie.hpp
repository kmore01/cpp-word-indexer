#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// Trie estándar: cada nodo representa un carácter y una palabra se almacena
// como el camino desde la raíz hasta un nodo marcado como hoja. En cada hoja
// se guardan las posiciones (línea, palabra) donde aparece esa palabra.
class NodoTrie {
   private:
    // La clave es el carácter de la arista y el valor, el nodo hijo.
    std::unordered_map<char, NodoTrie*> hijos;
    bool hoja = false;
    std::vector<std::pair<int, int>> lineaPos;
    static int nodos;

    // Recorre el subárbol acumulando el número de palabras y la suma de sus
    // profundidades, de donde se obtiene la profundidad media.
    void calcularProfundidadesAux(int profundidadActual, int& contador,
                                  long long& suma) const;

    // Índice de la palabra con menos ocurrencias dentro del top-k actual.
    int indexLessOcurr(const std::vector<std::pair<std::string, int>>& v);

    int contiene(const std::vector<std::pair<std::string, int>>& v,
                 const std::string& w);

    std::vector<std::string> toVectorString(
        const std::vector<std::pair<std::string, int>>& v);

   public:
    NodoTrie();

    explicit NodoTrie(bool h);

    ~NodoTrie();

    // Devuelve todas las palabras que empiezan por el prefijo w.
    std::vector<std::string> contarPalabrasPrefijo(const std::string& w,
                                                   int& nv);

    // Devuelve las 5 palabras más frecuentes que empiezan por w.
    std::vector<std::string> obtenerTop5MasFrecuentes(const std::string& w,
                                                      int& nv);

    void completarDFS(NodoTrie* nodo, std::string palabraActual,
                      std::vector<std::string>& resultados, int& nv);

    void completarDFS2(NodoTrie* nodo, std::string palabraActual,
                       std::vector<std::pair<std::string, int>>& resultados,
                       int& nv);

    NodoTrie* obtenerNodo(char c) const;

    NodoTrie* obtenerNodo(std::string w, int& nv);

    // Crea el hijo asociado al carácter c si no existe.
    void insertar(char c);

    // Inserta una aparición de la palabra w en (línea l, posición p).
    void insertarPalabra(std::string w, int l, int p);

    // Devuelve las posiciones donde aparece w, o vacío si no existe.
    std::vector<std::pair<int, int>> buscarPalabra(std::string w, int& nv);

    // Busca una secuencia de palabras consecutivas dentro de una misma línea.
    std::vector<std::pair<int, int>> buscarFrase(
        const std::vector<std::string>& frase, int& nv);

    // Getters y setters
    bool getHoja() const;

    static int getNodos();

    std::unordered_map<char, NodoTrie*>& getHijos();

    std::size_t calcularMemoria() const;

    double calcularProfundidadMedia() const;
};
