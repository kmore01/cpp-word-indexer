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

vector<string> NodoTrie::completarPrefijo(const string& w) {
    vector<string> resultados;
    NodoTrie* nodoPrefijo = obtenerNodo(w);
    if (nodoPrefijo != NULL) {
        string prefijoActual = w;
        completarDFS(nodoPrefijo, prefijoActual, resultados);
    }
    return resultados;
}

void NodoTrie::completarDFS(NodoTrie* nodo, string& palabraActual,
                            vector<string>& resultados) {
    if (nodo->hoja) {
        resultados.push_back(palabraActual);
    }
    for (auto& par : nodo->hijos) {
        palabraActual.push_back(par.first);  // Modificar in-place
        completarDFS(par.second, palabraActual, resultados);
        palabraActual.pop_back();  // Backtrack
    }
}

NodoTrie* NodoTrie::obtenerNodo(char c) const {
    auto it = hijos.find(c);
    if (it != hijos.end()) {
        return it->second;
    }
    return NULL;
}

NodoTrie* NodoTrie::obtenerNodo(string w) {
    // Caso base
    if (w.length() == 0) return this;
    // Caso recursivo
    auto it = hijos.find(w[0]);
    if (it == hijos.end()) {
        return NULL;
    } else {
        string w2 = w.substr(1);             // desde el indice 1 hasta el final
        return it->second->obtenerNodo(w2);  // Recursion en el hijo
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

vector<pair<int, int>> NodoTrie::buscarPalabra(string w) {
    NodoTrie* palabra = obtenerNodo(w);
    if (palabra != NULL && palabra->hoja) {
        return palabra->lineaPos;
    }
    return vector<pair<int, int>>();  // vector vacio si no se encuentra
}

vector<pair<int, int>> NodoTrie::buscarFrase(const vector<string>& frase) {
    if (frase.empty()) return vector<pair<int, int>>();
    // guardo todas las ocurrencias de cada palabra de la frase
    vector<vector<pair<int, int>>> ocurrenciasPalabras(frase.size());
    for (int i = 0; i < frase.size(); ++i) {
        vector<pair<int, int>> occs = buscarPalabra(frase[i]);
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

bool NodoTrie::contienePalabra(string w) {
    NodoTrie* nodo = obtenerNodo(w);
    if (nodo == NULL) {
        return false;
    } else {
        return nodo->hoja;
    }
}

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