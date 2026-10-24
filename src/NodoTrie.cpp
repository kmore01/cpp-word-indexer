#include "trie/NodoTrie.hpp"

#include <climits>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

int NodoTrie::nodos = 0;

NodoTrie::NodoTrie() { ++nodos; }

NodoTrie::NodoTrie(bool h) : hoja{h} { ++nodos; }

NodoTrie::~NodoTrie() {
    for (auto& par : hijos) {
        delete par.second;
    }
    hijos.clear();
}

std::vector<std::string> NodoTrie::contarPalabrasPrefijo(const std::string& w,
                                                         int& nv) {
    std::vector<std::string> resultados;
    NodoTrie* nodoPrefijo = obtenerNodo(w, nv);
    if (nodoPrefijo != nullptr) {
        completarDFS(nodoPrefijo, w, resultados, nv);
        --nv;  // El nodo del prefijo se contabilizó dos veces.
    }
    return resultados;
}

std::vector<std::string> NodoTrie::obtenerTop5MasFrecuentes(
    const std::string& w, int& nv) {
    // Se reservan siempre 5 huecos; los no utilizados quedan vacíos.
    const int n = 5;
    std::vector<std::pair<std::string, int>> resultados(n, {"", -1});
    NodoTrie* nodoPrefijo = obtenerNodo(w, nv);
    if (nodoPrefijo != nullptr) {
        completarDFS2(nodoPrefijo, w, resultados, nv);
        --nv;  // El nodo del prefijo se contabilizó dos veces.
    }
    return toVectorString(resultados);
}

void NodoTrie::completarDFS(NodoTrie* nodo, std::string palabraActual,
                            std::vector<std::string>& resultados, int& nv) {
    ++nv;
    if (nodo->hoja) {
        resultados.push_back(palabraActual);
    }
    for (auto& par : nodo->hijos) {
        completarDFS(par.second, palabraActual + par.first, resultados, nv);
    }
}

void NodoTrie::completarDFS2(
    NodoTrie* nodo, std::string palabraActual,
    std::vector<std::pair<std::string, int>>& resultados, int& nv) {
    ++nv;
    if (nodo->hoja) {
        const int ocurrencias = static_cast<int>(nodo->lineaPos.size());
        const int idx = contiene(resultados, palabraActual);
        if (idx == -1) {
            // Se sustituye la palabra menos frecuente del top-k si la nueva
            // tiene más apariciones.
            const int idx2 = indexLessOcurr(resultados);
            if (resultados[idx2].second == -1 ||
                ocurrencias > resultados[idx2].second) {
                resultados[idx2] = {palabraActual, ocurrencias};
            }
        }
    }
    for (auto& par : nodo->hijos) {
        completarDFS2(par.second, palabraActual + par.first, resultados, nv);
    }
}

NodoTrie* NodoTrie::obtenerNodo(char c) const {
    auto it = hijos.find(c);
    if (it != hijos.end()) {
        return it->second;
    }
    return nullptr;
}

NodoTrie* NodoTrie::obtenerNodo(std::string w, int& nv) {
    ++nv;
    if (w.empty()) {
        return this;
    }
    auto it = hijos.find(w[0]);
    if (it == hijos.end()) {
        return nullptr;
    }
    const std::string resto = w.substr(1);
    return it->second->obtenerNodo(resto, nv);
}

void NodoTrie::insertar(char c) {
    auto it = hijos.find(c);
    if (it == hijos.end()) {
        hijos[c] = new NodoTrie();
    }
}

void NodoTrie::insertarPalabra(std::string w, int l, int p) {
    NodoTrie* aux = this;  // Se recorre el trie desde la raíz.
    for (char c : w) {
        NodoTrie* hijo = aux->obtenerNodo(c);
        if (hijo == nullptr) {
            aux->insertar(c);             // Crear el hijo si no existe.
            hijo = aux->obtenerNodo(c);   // Recuperar el nodo recién creado.
        }
        aux = hijo;
    }
    aux->hoja = true;
    aux->lineaPos.push_back({l, p});
}

std::vector<std::pair<int, int>> NodoTrie::buscarPalabra(std::string w,
                                                         int& nv) {
    nv = 0;
    NodoTrie* palabra = obtenerNodo(w, nv);
    if (palabra != nullptr && palabra->hoja) {
        return palabra->lineaPos;
    }
    return {};
}

std::vector<std::pair<int, int>> NodoTrie::buscarFrase(
    const std::vector<std::string>& frase, int& nv) {
    nv = 0;
    if (frase.empty()) return {};

    // Ocurrencias de cada palabra de la frase, en el mismo orden.
    std::vector<std::vector<std::pair<int, int>>> ocurrenciasPalabras(
        frase.size());
    for (std::size_t i = 0; i < frase.size(); ++i) {
        int aux = 0;
        std::vector<std::pair<int, int>> occs = buscarPalabra(frase[i], aux);
        nv += aux;
        if (occs.empty()) return {};
        ocurrenciasPalabras[i] = occs;
    }

    std::vector<std::pair<int, int>> resultados;
    // Se parte de cada aparición de la primera palabra y se comprueba que las
    // siguientes ocupan posiciones consecutivas en la misma línea.
    for (const std::pair<int, int>& occ1 : ocurrenciasPalabras[0]) {
        const int linea = occ1.first;
        const int posInicial = occ1.second;
        bool fraseEncontrada = true;

        for (std::size_t i = 1; i < frase.size(); ++i) {
            const int posEsperada = posInicial + static_cast<int>(i);
            bool encontrada = false;
            for (const std::pair<int, int>& occ : ocurrenciasPalabras[i]) {
                if (occ.first == linea && occ.second == posEsperada) {
                    encontrada = true;
                    break;
                }
            }
            if (!encontrada) {
                fraseEncontrada = false;
                break;
            }
        }

        if (fraseEncontrada) {
            resultados.push_back({linea, posInicial});
        }
    }
    return resultados;
}

bool NodoTrie::getHoja() const { return hoja; }

int NodoTrie::getNodos() { return nodos; }

std::unordered_map<char, NodoTrie*>& NodoTrie::getHijos() { return hijos; }

std::size_t NodoTrie::calcularMemoria() const {
    std::size_t total = sizeof(NodoTrie);
    total += lineaPos.capacity() * sizeof(std::pair<int, int>);
    // Estimación del coste de los buckets y de cada entrada del map.
    total += hijos.bucket_count() * sizeof(void*);
    total += hijos.size() * (sizeof(char) + sizeof(NodoTrie*) + 16);
    for (const auto& par : hijos) {
        total += par.second->calcularMemoria();
    }
    return total;
}

void NodoTrie::calcularProfundidadesAux(int profundidadActual, int& contador,
                                        long long& suma) const {
    if (hoja) {
        ++contador;
        suma += profundidadActual;
    }
    for (const auto& par : hijos) {
        if (par.second != nullptr) {
            par.second->calcularProfundidadesAux(profundidadActual + 1,
                                                 contador, suma);
        }
    }
}

int NodoTrie::indexLessOcurr(
    const std::vector<std::pair<std::string, int>>& v) {
    const std::size_t n = v.size();
    int idx = -1;
    int less = INT_MAX;
    for (std::size_t i = 0; i < n; ++i) {
        if (v[i].second < less) {
            idx = static_cast<int>(i);
            less = v[i].second;
        }
    }
    return idx;
}

int NodoTrie::contiene(const std::vector<std::pair<std::string, int>>& v,
                       const std::string& w) {
    for (std::size_t i = 0; i < v.size(); ++i) {
        if (v[i].first == w) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

std::vector<std::string> NodoTrie::toVectorString(
    const std::vector<std::pair<std::string, int>>& v) {
    std::vector<std::string> resultado;
    resultado.reserve(v.size());
    for (const auto& par : v) {
        resultado.push_back(par.first);
    }
    return resultado;
}

double NodoTrie::calcularProfundidadMedia() const {
    int contador = 0;
    long long suma = 0;
    calcularProfundidadesAux(1, contador, suma);
    if (contador == 0) return 0.0;
    return static_cast<double>(suma) / contador;
}
