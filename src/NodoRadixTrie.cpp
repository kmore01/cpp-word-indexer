#include "trie/NodoRadixTrie.hpp"

#include <algorithm>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

int NodoRadixTrie::nodos = 0;

NodoRadixTrie::NodoRadixTrie() { ++nodos; }

NodoRadixTrie::NodoRadixTrie(bool h) : hoja{h} { ++nodos; }

NodoRadixTrie::~NodoRadixTrie() {
    for (auto& par : hijos) {
        delete par.second;
    }
    hijos.clear();
}

std::string NodoRadixTrie::prefijoCom(const std::string& s1,
                                      const std::string& s2) {
    std::size_t i = 0;
    const std::size_t minLong = std::min(s1.size(), s2.size());
    while (i < minLong && s1[i] == s2[i]) {
        ++i;
    }
    return s1.substr(0, i);
}

std::vector<std::string> NodoRadixTrie::contarPalabrasPrefijo(
    const std::string& w, int& nv) {
    std::vector<std::string> res;
    nv = 0;
    completarPrefijoAux(w, "", res, nv);
    return res;
}

int NodoRadixTrie::partition(std::vector<std::pair<std::string, int>>& vec,
                             int left, int right) {
    const int pivotValue = vec[right].second;
    int idx = left;
    for (int j = left; j < right; ++j) {
        if (vec[j].second >= pivotValue) {  // Orden descendente.
            std::swap(vec[idx], vec[j]);
            ++idx;
        }
    }
    std::swap(vec[idx], vec[right]);
    return idx;
}

std::pair<std::string, int> NodoRadixTrie::encontrarIesimoMayor(
    std::vector<std::pair<std::string, int>>& vec, int i) {
    if (i < 0 || static_cast<std::size_t>(i) >= vec.size()) {
        return {};
    }
    int left = 0;
    int right = static_cast<int>(vec.size()) - 1;
    while (left <= right) {
        const int pivotIndex = partition(vec, left, right);

        if (pivotIndex == i) {
            return vec[i];  // Se encontró el i-ésimo mayor.
        }
        if (pivotIndex < i) {
            left = pivotIndex + 1;
        } else {
            right = pivotIndex - 1;
        }
    }
    return vec[i];
}

std::vector<std::string> NodoRadixTrie::obtenerTop5MasFrecuentes(
    const std::string& w, int& nv) {
    // Siempre se devuelven 5 huecos; los no utilizados quedan vacíos.
    const int n = 5;
    std::vector<std::pair<std::string, int>> res;
    std::vector<std::string> ret(n);
    nv = 0;
    completarPrefijoAux2(w, "", res, nv);
    for (int i = 0; i < n; ++i) {
        ret[i] = encontrarIesimoMayor(res, i).first;
    }
    return ret;
}

void NodoRadixTrie::completarPrefijoAux2(
    const std::string& prefijoBuscado, std::string palabraActual,
    std::vector<std::pair<std::string, int>>& resultados, int& nv) {
    ++nv;
    // Si ya se consumió todo el prefijo, se recogen todas las palabras.
    if (prefijoBuscado.empty()) {
        if (hoja) {
            resultados.push_back({palabraActual, lineaPos.size()});
        }
        for (auto& par : hijos) {
            const std::string nuevaPalabra = palabraActual + par.first;
            par.second->completarPrefijoAux2("", nuevaPalabra, resultados, nv);
        }
        return;
    }
    for (auto& par : hijos) {
        const std::string palabraHijo = par.first;
        const std::string prefijo = prefijoCom(palabraHijo, prefijoBuscado);
        if (prefijo.empty()) continue;

        if (prefijo == palabraHijo) {
            // La arista completa coincide: se sigue bajando por el subárbol.
            const std::string resto = prefijoBuscado.substr(prefijo.size());
            par.second->completarPrefijoAux2(resto,
                                             palabraActual + palabraHijo,
                                             resultados, nv);
        } else if (prefijo == prefijoBuscado) {
            // El prefijo termina dentro de esta arista: se recoge todo el
            // subárbol a partir del nodo hijo.
            const std::string nuevaPalabra = palabraActual + palabraHijo;
            par.second->completarPrefijoAux2("", nuevaPalabra, resultados, nv);
        }
    }
}

void NodoRadixTrie::completarPrefijoAux(
    const std::string& prefijoBuscado, std::string palabraActual,
    std::vector<std::string>& resultados, int& nv) {
    ++nv;
    // Si ya se consumió todo el prefijo, se recogen todas las palabras.
    if (prefijoBuscado.empty()) {
        if (hoja) {
            resultados.push_back(palabraActual);
        }
        for (auto& par : hijos) {
            const std::string nuevaPalabra = palabraActual + par.first;
            par.second->completarPrefijoAux("", nuevaPalabra, resultados, nv);
        }
        return;
    }
    for (auto& par : hijos) {
        const std::string palabraHijo = par.first;
        const std::string prefijo = prefijoCom(palabraHijo, prefijoBuscado);
        if (prefijo.empty()) continue;

        if (prefijo == palabraHijo) {
            // La arista completa coincide: se sigue bajando por el subárbol.
            const std::string resto = prefijoBuscado.substr(prefijo.size());
            par.second->completarPrefijoAux(resto, palabraActual + palabraHijo,
                                            resultados, nv);
        } else if (prefijo == prefijoBuscado) {
            // El prefijo termina dentro de esta arista: se recoge todo el
            // subárbol a partir del nodo hijo.
            const std::string nuevaPalabra = palabraActual + palabraHijo;
            par.second->completarPrefijoAux("", nuevaPalabra, resultados, nv);
        }
    }
}

void NodoRadixTrie::completarDFS(NodoRadixTrie* nodo,
                                 std::string& palabraActual,
                                 std::vector<std::string>& resultados,
                                 int& nv) {
    ++nv;
    if (nodo->hoja) {
        resultados.push_back(palabraActual);
    }
    for (auto& par : nodo->hijos) {
        const std::size_t tamañoOriginal = palabraActual.size();
        palabraActual += par.first;
        completarDFS(par.second, palabraActual, resultados, nv);
        palabraActual.resize(tamañoOriginal);  // Backtracking.
    }
}

NodoRadixTrie* NodoRadixTrie::obtenerNodo(const std::string& w, int& nv) {
    ++nv;
    if (w.empty()) {
        return this;
    }
    for (auto& par : hijos) {
        const std::string palabraHijo = par.first;
        const std::string prefijo = prefijoCom(palabraHijo, w);
        if (prefijo.empty()) continue;

        // w = "blanco", palabraHijo = "bla": se sigue por el subárbol.
        if (prefijo == palabraHijo) {
            const std::string resto = w.substr(prefijo.size());
            return par.second->obtenerNodo(resto, nv);
        }
        // w = "bla", palabraHijo = "blanco": sirve para el autocompletado.
        if (prefijo == w && prefijo.size() < palabraHijo.size()) {
            return par.second;
        }
    }
    return nullptr;
}

void NodoRadixTrie::insertarPalabra(const std::string& w, int l, int p) {
    if (w.empty()) {
        hoja = true;
        lineaPos.push_back({l, p});
        return;
    }
    for (auto& par : hijos) {
        const std::string palabraHijo = par.first;
        const std::string prefijo = prefijoCom(palabraHijo, w);
        if (prefijo.empty()) continue;

        if (prefijo == palabraHijo) {
            // w = "testing", palabraHijo = "test": se baja con el resto.
            const std::string resto = w.substr(palabraHijo.size());
            par.second->insertarPalabra(resto, l, p);
            return;
        }
        if (prefijo == w) {
            // w = "blanco", palabraHijo = "blancos": se inserta un nodo
            // intermedio que pasa a ser hoja y se reconecta el subárbol.
            NodoRadixTrie* nodoViejo = par.second;
            NodoRadixTrie* nodoNuevo = new NodoRadixTrie(true);
            nodoNuevo->lineaPos.push_back({l, p});
            const std::string resto = palabraHijo.substr(prefijo.size());
            nodoNuevo->hijos[resto] = nodoViejo;
            hijos.erase(par.first);
            hijos[prefijo] = nodoNuevo;
            return;
        }
        if (prefijo.size() < palabraHijo.size() &&
            prefijo.size() < w.size()) {
            // palabraHijo = "blanco", w = "blasfemia": se crea un nodo
            // intermedio del que cuelgan ambas ramas.
            NodoRadixTrie* nodoIntermedio = new NodoRadixTrie();
            NodoRadixTrie* nodoViejo = par.second;
            const std::string resto = palabraHijo.substr(prefijo.size());
            nodoIntermedio->hijos[resto] = nodoViejo;
            const std::string restoW = w.substr(prefijo.size());
            NodoRadixTrie* nodoNuevo = new NodoRadixTrie(true);
            nodoNuevo->lineaPos.push_back({l, p});
            nodoIntermedio->hijos[restoW] = nodoNuevo;
            hijos.erase(par.first);
            hijos[prefijo] = nodoIntermedio;
            return;
        }
    }
    // No hay ningún prefijo en común con las aristas existentes.
    NodoRadixTrie* nodo = new NodoRadixTrie(true);
    nodo->lineaPos.push_back({l, p});
    hijos[w] = nodo;
}

std::vector<std::pair<int, int>> NodoRadixTrie::buscarPalabra(
    const std::string& w, int& nv) {
    if (w.empty()) {
        ++nv;
        if (hoja) {
            return lineaPos;
        }
        return {};
    }
    ++nv;
    for (auto& par : hijos) {
        const std::string palabraHijo = par.first;
        const std::string prefijo = prefijoCom(palabraHijo, w);
        if (prefijo.empty()) continue;

        // w = "blanco", palabraHijo = "bla": se sigue por el subárbol.
        if (prefijo == palabraHijo) {
            const std::string resto = w.substr(prefijo.size());
            int nvHijo = 0;
            std::vector<std::pair<int, int>> res =
                par.second->buscarPalabra(resto, nvHijo);
            nv += nvHijo;
            return res;
        }
        // w = "bla", palabraHijo = "blanco": la palabra no existe.
        if (prefijo == w) {
            return {};
        }
    }
    return {};
}

std::vector<std::pair<int, int>> NodoRadixTrie::buscarFrase(
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

    std::vector<std::pair<int, int>> res;
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
            res.push_back({linea, posInicial});
        }
    }
    return res;
}

bool NodoRadixTrie::getHoja() const { return hoja; }

int NodoRadixTrie::getNodos() { return nodos; }

std::unordered_map<std::string, NodoRadixTrie*>& NodoRadixTrie::getHijos() {
    return hijos;
}

std::size_t NodoRadixTrie::calcularMemoria() const {
    std::size_t total = sizeof(NodoRadixTrie);
    total += lineaPos.capacity() * sizeof(std::pair<int, int>);
    // Estimación del coste de los buckets del map.
    total += hijos.bucket_count() * sizeof(void*);
    for (const auto& par : hijos) {
        total += sizeof(std::string);
        total += par.first.capacity() * sizeof(char);
        total += sizeof(NodoRadixTrie*);
        total += 16;  // Overhead aproximado de cada entrada del map.
        total += par.second->calcularMemoria();
    }
    return total;
}

void NodoRadixTrie::calcularProfundidadesAux(int profundidadActual,
                                             int& contador,
                                             long long& suma) const {
    if (hoja) {
        ++contador;
        suma += profundidadActual;
    }
    for (const auto& par : hijos) {
        if (par.second != nullptr) {
            // Cada arista cuenta como un nivel, no como su longitud.
            par.second->calcularProfundidadesAux(profundidadActual + 1,
                                                 contador, suma);
        }
    }
}

double NodoRadixTrie::calcularProfundidadMedia() const {
    int contador = 0;
    long long suma = 0;
    calcularProfundidadesAux(1, contador, suma);
    if (contador == 0) return 0.0;
    return static_cast<double>(suma) / contador;
}

void NodoRadixTrie::calcularProfundidadMaximaAux(int profundidadActual,
                                                 int& maxProf) const {
    if (hoja) {
        maxProf = std::max(maxProf, profundidadActual);
    }
    for (const auto& par : hijos) {
        if (par.second != nullptr) {
            par.second->calcularProfundidadMaximaAux(profundidadActual + 1,
                                                     maxProf);
        }
    }
}

int NodoRadixTrie::calcularProfundidadMaxima() const {
    int maxProf = 0;
    calcularProfundidadMaximaAux(0, maxProf);
    return maxProf;
}

void NodoRadixTrie::contarPalabrasAux(int& contador) const {
    if (hoja) {
        ++contador;
    }
    for (const auto& par : hijos) {
        if (par.second != nullptr) {
            par.second->contarPalabrasAux(contador);
        }
    }
}

int NodoRadixTrie::contarPalabras() const {
    int contador = 0;
    contarPalabrasAux(contador);
    return contador;
}
