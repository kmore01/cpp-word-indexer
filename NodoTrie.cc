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

void NodoTrie::insertar(char c) {
    auto it = hijos.find(c);
    if (it == hijos.end()) {
        hijos[c] = new NodoTrie();
    }
}

void NodoTrie::insertar_word(string w, int l, int p) {
    NodoTrie* aux = this;  // empezamos desde la raiz
    for (char c : w) {
        NodoTrie* hijo = aux->obtener(c);
        if (!hijo) {
            aux->insertar(c);        // crear el hijo si no existe
            hijo = aux->obtener(c);  // obtener el nuevo hijo
        }
        aux = hijo;  // avanzamos al hijo
    }
    aux->hoja = true;  // marcar nodo final como hoja
    aux->lineaPos.push_back({l, p});
}

vector<pair<int, int>> NodoTrie::buscar_word(string w) {
    NodoTrie* word = obtener(w);
    if (word != NULL && word->hoja) {
        return word->lineaPos;
    }
    return vector<pair<int, int>>();  // lista vacía si no se encuentra
}

bool NodoTrie::esHoja() const { return hoja; }

int NodoTrie::getNodos() { return nodos; }

bool NodoTrie::contienePalabra(string w) {
    NodoTrie* nodo = obtener(w);
    if (nodo == NULL) {
        return false;
    } else {
        return nodo->hoja;
    }
}

NodoTrie* NodoTrie::obtener(char c) const {
    auto it = hijos.find(c);
    if (it != hijos.end()) {
        return it->second;
    }
    return NULL;
}

NodoTrie* NodoTrie::obtener(string w) {
    // Caso base
    if (w.length() == 0) return this;
    // Caso recursivo
    auto it = hijos.find(w[0]);
    if (it == hijos.end()) {
        return NULL;
    } else {
        string w2 = w.substr(1);         // desde el índice 1 hasta el final
        return it->second->obtener(w2);  // Recursión en el hijo
    }
}

unordered_map<char, NodoTrie*>& NodoTrie::getHijos() { return hijos; }

vector<string> NodoTrie::completarPrefijo(const string& w) {
    vector<string> resultados;
    NodoTrie* nodoPrefijo = obtener(w);

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

    for (auto& [letra, hijo] : nodo->hijos) {
        palabraActual.push_back(letra);  // Modificar in-place
        completarDFS(hijo, palabraActual, resultados);
        palabraActual.pop_back();  // Backtrack
    }
}

vector<pair<int, int>> NodoTrie::buscarFrase(const vector<string>& frase) {
    if (frase.empty()) return {};

    // guardo todas las ocurrencias de cada palabra de la frase
    vector<vector<pair<int, int>>> ocurrenciasPalabras(frase.size());
    for (int i = 0; i < frase.size(); ++i) {
        vector<pair<int, int>> occs = buscar_word(frase[i]);
        if (occs.empty()) return {};  // Si falta una palabra
        ocurrenciasPalabras[i] = occs;
    }

    vector<pair<int, int>> resultados;

    // Para cada ocurrencia de la primera palabra
    for (const pair<int, int>& occ1 : ocurrenciasPalabras[0]) {
        int linea = occ1.first;
        int posInicial = occ1.second;
        bool fraseEncontrada = true;

        // Verificar que las siguientes palabras están en posiciones
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