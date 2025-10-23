#include "NodoTrie.hh"

int NodoTrie::nodos = 0;

// Constructor
NodoTrie::NodoTrie() {
    hoja = false;
    ++nodos;
}

NodoTrie::NodoTrie(bool h) {
    hoja = h;
    ++nodos;
}

NodoTrie::~NodoTrie() {
    for (auto& par : hijos) {
        delete par.second;
    }
    hijos.clear();
}

vector<string> NodoTrie::completarPrefijo(const string& w, int& nv) {
    vector<string> resultados;
    NodoTrie* nodoPrefijo = obtenerNodo(w, nv);
    if (nodoPrefijo != NULL) {
        completarDFS(nodoPrefijo, w, resultados, nv);
        --nv;  // nodo prefijo 2 veces
    }
    return resultados;
}

vector<string> NodoTrie::completarPrefijo2(const string& w, int& nv, int n) {
    // inicializacion necesaria
    vector<pair<string, int>> resultados(n, {"", -1});
    NodoTrie* nodoPrefijo = obtenerNodo(w, nv);
    if (nodoPrefijo != NULL) {
        completarDFS2(nodoPrefijo, w, resultados, nv);
        --nv;  // nodo prefijo 2 veces
    }
    return toVectorString(resultados);
}

void NodoTrie::completarDFS(NodoTrie* nodo, string palabraActual,
                            vector<string>& resultados, int& nv) {
    if (resultados.size() < 5) {
        ++nv;
        if (nodo->hoja) {
            resultados.push_back(palabraActual);
        }
        for (auto& par : nodo->hijos) {
            completarDFS(par.second, palabraActual + par.first, resultados, nv);
        }
    }
}

void NodoTrie::completarDFS2(NodoTrie* nodo, string palabraActual,
                             vector<pair<string, int>>& resultados, int& nv) {
    ++nv;
    if (nodo->hoja) {
        int ocurrencias = nodo->lineaPos.size();
        int idx = contiene(resultados, palabraActual);
        if (idx == -1) {  // creo que no hace falta pero por si acaso
            int idx2 = indexLessOcurr(resultados);
            if (resultados[idx2].second == -1 or
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
    return NULL;
}

NodoTrie* NodoTrie::obtenerNodo(string w, int& nv) {
    ++nv;
    // Caso base
    if (w.length() == 0) return this;
    // Caso recursivo
    auto it = hijos.find(w[0]);
    if (it == hijos.end()) {
        return NULL;
    } else {
        string w2 = w.substr(1);  // desde el indice 1 hasta el final
        return it->second->obtenerNodo(w2, nv);  // Recursion en el hijo
    }
}

void NodoTrie::insertar(char c) {
    auto it = hijos.find(c);
    if (it == hijos.end()) {
        hijos[c] = new NodoTrie();
    }
}

void NodoTrie::insertarPalabra(string w, int l, int p) {
    NodoTrie* aux = this;  // empezamos desde la raiz
    for (char c : w) {
        NodoTrie* hijo = aux->obtenerNodo(c);
        if (hijo == NULL) {
            aux->insertar(c);            // crear el hijo si no existe
            hijo = aux->obtenerNodo(c);  // obtener el nuevo hijo
        }
        aux = hijo;  // avanzamos al hijo
    }
    aux->hoja = true;  // marcar nodo final como hoja
    aux->lineaPos.push_back({l, p});
}

vector<pair<int, int>> NodoTrie::buscarPalabra(string w, int& nv) {
    nv = 0;
    NodoTrie* palabra = obtenerNodo(w, nv);
    if (palabra != NULL && palabra->hoja) {
        return palabra->lineaPos;
    }
    return vector<pair<int, int>>();  // vector vacio si no se encuentra
}

vector<pair<int, int>> NodoTrie::buscarFrase(const vector<string>& frase,
                                             int& nv) {
    nv = 0;
    if (frase.empty()) return vector<pair<int, int>>();
    // guardo todas las ocurrencias de cada palabra de la frase
    vector<vector<pair<int, int>>> ocurrenciasPalabras(frase.size());
    for (int i = 0; i < frase.size(); ++i) {
        int aux = 0;
        vector<pair<int, int>> occs = buscarPalabra(frase[i], aux);
        nv += aux;
        if (occs.empty()) return {};  // Si falta una palabra
        ocurrenciasPalabras[i] = occs;
    }
    vector<pair<int, int>> resultados;
    // Para cada ocurrencia de la primera palabra
    for (const pair<int, int>& occ1 : ocurrenciasPalabras[0]) {
        int linea = occ1.first;
        int posInicial = occ1.second;
        bool fraseEncontrada = true;
        // Verificar que las siguientes palabras estan en posiciones
        // consecutivas en la misma linea
        for (int i = 1; i < frase.size(); i++) {
            int posEsperada = posInicial + i;
            bool encontrada = false;
            // Buscar en las ocurrencias de la palabra i
            for (const pair<int, int>& occ : ocurrenciasPalabras[i]) {
                if (occ.first == linea and occ.second == posEsperada) {
                    encontrada = true;
                    break;
                }
            }
            if (not encontrada) {
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

unordered_map<char, NodoTrie*>& NodoTrie::getHijos() { return hijos; }

size_t NodoTrie::calcularMemoria() const {
    size_t total = sizeof(NodoTrie);  // tamaño base del nodo
    // Memoria del vector
    total += lineaPos.capacity() * sizeof(pair<int, int>);
    // Memoria del unordered_map (aproximado)
    total += hijos.bucket_count() * sizeof(void*);  // buckets
    total += hijos.size() *
             (sizeof(char) + sizeof(NodoTrie*) + 16);  // entries + overhead
    // Recursivo: memoria de los hijos
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
        if (par.second != NULL) {
            par.second->calcularProfundidadesAux(profundidadActual + 1,
                                                 contador, suma);
        }
    }
}

int NodoTrie::indexLessOcurr(const vector<pair<string, int>>& v) {
    int n = v.size(), idx = -1, less = INT_MAX;
    for (int i = 0; i < n; ++i) {
        pair<string, int> aux = v[i];
        if (aux.second < less) {
            idx = i;
            less = aux.second;
        }
    }
    return idx;
}

int NodoTrie::contiene(const vector<pair<string, int>>& v, const string& w) {
    int n = v.size();
    for (int i = 0; i < n; ++i) {
        pair<string, int> aux = v[i];
        if (aux.first == w) {
            return i;
        }
    }
    return -1;  // v no contiene w
}

vector<string> NodoTrie::toVectorString(const vector<pair<string, int>>& v) {
    vector<string> resultado;
    resultado.reserve(v.size());  // Pre-reservar espacio
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