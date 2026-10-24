// Pruebas de humo sobre la API pública de ambos tries. No dependen de ningún
// framework externo: si una comprobación falla, el proceso termina con error.

#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "trie/NodoRadixTrie.hpp"
#include "trie/NodoTrie.hpp"

namespace {

void require(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        std::abort();
    }
}

template <typename T>
void probarBusqueda(T& trie) {
    trie.insertarPalabra("hola", 1, 0);
    trie.insertarPalabra("hola", 3, 2);
    trie.insertarPalabra("mundo", 2, 1);

    int nv = 0;
    const std::vector<std::pair<int, int>> posiciones =
        trie.buscarPalabra("hola", nv);
    require(posiciones.size() == 2,
            "buscarPalabra debe devolver 2 posiciones");
    require(posiciones[0] == std::make_pair(1, 0),
            "primera posición incorrecta");
    require(posiciones[1] == std::make_pair(3, 2),
            "segunda posición incorrecta");

    nv = 0;
    require(trie.buscarPalabra("adios", nv).empty(),
            "una palabra inexistente no debe tener posiciones");
}

template <typename T>
void probarFrase(T& trie) {
    trie.insertarPalabra("hola", 1, 0);
    trie.insertarPalabra("mundo", 1, 1);
    trie.insertarPalabra("mundo", 2, 0);

    const std::vector<std::string> frase = {"hola", "mundo"};
    int nv = 0;
    const std::vector<std::pair<int, int>> resultados =
        trie.buscarFrase(frase, nv);
    require(resultados.size() == 1,
            "la frase debe aparecer una sola vez");
    require(resultados[0] == std::make_pair(1, 0),
            "línea/posición de frase incorrecta");

    nv = 0;
    require(trie.buscarFrase({}, nv).empty(),
            "una frase vacía no debe coincidir");
}

template <typename T>
void probarAutocompletado(T& trie) {
    trie.insertarPalabra("casa", 1, 0);
    trie.insertarPalabra("caso", 1, 1);
    trie.insertarPalabra("caso", 2, 0);
    trie.insertarPalabra("perro", 1, 2);

    int nv = 0;
    const std::vector<std::string> palabras =
        trie.contarPalabrasPrefijo("cas", nv);
    require(palabras.size() == 2, "el prefijo 'cas' debe tener 2 palabras");

    nv = 0;
    const std::vector<std::string> top =
        trie.obtenerTop5MasFrecuentes("cas", nv);
    bool encontrada = false;
    for (const std::string& palabra : top) {
        if (!palabra.empty()) encontrada = true;
    }
    require(encontrada, "el top-5 debe contener alguna palabra");
}

template <typename T>
void probarClear(T& trie) {
    trie.insertarPalabra("hola", 1, 0);
    trie.clear();

    int nv = 0;
    require(trie.buscarPalabra("hola", nv).empty(),
            "clear debe eliminar las palabras anteriores");

    trie.insertarPalabra("adios", 2, 0);
    require(trie.buscarPalabra("adios", nv).size() == 1,
            "el trie debe seguir usable después de clear");
}

template <typename T>
void ejecutarPruebas(const char* nombre) {
    {
        T trie;
        probarBusqueda(trie);
    }
    {
        T trie;
        probarFrase(trie);
    }
    {
        T trie;
        probarAutocompletado(trie);
    }
    {
        T trie;
        probarClear(trie);
    }
    std::cout << "OK: " << nombre << '\n';
}

}  // namespace

int main() {
    ejecutarPruebas<NodoTrie>("NodoTrie");
    ejecutarPruebas<NodoRadixTrie>("NodoRadixTrie");
    std::cout << "Todas las pruebas pasaron.\n";
    return 0;
}
