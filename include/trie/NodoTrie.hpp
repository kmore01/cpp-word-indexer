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
    int indexLessOcurr(
        const std::vector<std::pair<std::string, int>>& v) const;

    int contiene(const std::vector<std::pair<std::string, int>>& v,
                 const std::string& w) const;

    std::vector<std::string> toVectorString(
        const std::vector<std::pair<std::string, int>>& v) const;

   public:
    NodoTrie();

    explicit NodoTrie(bool h);

    ~NodoTrie();

    // Libera todos los nodos hijos y deja este nodo como raíz vacía.
    void clear();

    // Los nodos poseen punteros propietarios: copiar un trie provocaría un
    // doble delete, así que la copia queda prohibida.
    NodoTrie(const NodoTrie&) = delete;
    NodoTrie& operator=(const NodoTrie&) = delete;

    // Devuelve todas las palabras que empiezan por el prefijo w.
    std::vector<std::string> contarPalabrasPrefijo(const std::string& w,
                                                   int& nv) const;

    // Devuelve las 5 palabras más frecuentes que empiezan por w.
    std::vector<std::string> obtenerTop5MasFrecuentes(const std::string& w,
                                                      int& nv) const;

    void completarDFS(const NodoTrie* nodo, std::string palabraActual,
                      std::vector<std::string>& resultados, int& nv) const;

    void completarDFS2(const NodoTrie* nodo, std::string palabraActual,
                       std::vector<std::pair<std::string, int>>& resultados,
                       int& nv) const;

    NodoTrie* obtenerNodo(char c) const;

    const NodoTrie* obtenerNodo(const std::string& w, int& nv) const;

    // Crea el hijo asociado al carácter c si no existe.
    void insertar(char c);

    // Inserta una aparición de la palabra w en (línea l, posición p).
    void insertarPalabra(const std::string& w, int l, int p);

    // Devuelve las posiciones donde aparece w, o vacío si no existe.
    std::vector<std::pair<int, int>> buscarPalabra(const std::string& w,
                                                   int& nv) const;

    // Busca una secuencia de palabras consecutivas dentro de una misma línea.
    std::vector<std::pair<int, int>> buscarFrase(
        const std::vector<std::string>& frase, int& nv) const;

    // Getters y setters
    bool getHoja() const;

    static int getNodos();

    const std::unordered_map<char, NodoTrie*>& getHijos() const;

    std::size_t calcularMemoria() const;

    double calcularProfundidadMedia() const;
};
