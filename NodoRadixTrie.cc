#include "NodoRadixTrie.hh"

int NodoRadixTrie::nodos = 0;

// Constructor
NodoRadixTrie::NodoRadixTrie() {
    hoja = false;
    ++nodos;
}

NodoRadixTrie::NodoRadixTrie(bool h) {
    hoja = h;
    ++nodos;
}

NodoRadixTrie::~NodoRadixTrie() {
    for (auto& par : hijos) {
        delete par.second;
    }
    hijos.clear();
}

string NodoRadixTrie::prefijoCom(const string& s1, const string& s2) {
    int i = 0, minLong = min(s1.length(), s2.length());
    while (i < minLong && s1[i] == s2[i]) {
        ++i;
    }
    return s1.substr(0, i);
}

vector<string> NodoRadixTrie::completarPrefijo(const string& w) {
    NodoRadixTrie* nodo = obtenerNodo(w);
    if (nodo == NULL) return vector<string>();  // no existe ese prefijo
    vector<string> res;
    string palabraActual = w;
    completarDFS(nodo, palabraActual, res);
    return res;
}

void NodoRadixTrie::completarDFS(NodoRadixTrie* nodo, string& palabraActual,
                                 vector<string>& resultados) {
    // Caso base: el nodo es hoja
    if (nodo->hoja) {
        resultados.push_back(palabraActual);
    }
    for (auto& par : nodo->hijos) {
        size_t tamañoOriginal = palabraActual.size();
        // Ponerle el sufijo completo
        palabraActual += par.first;
        completarDFS(par.second, palabraActual, resultados);
        // Backtracking
        palabraActual.resize(tamañoOriginal);
    }
}

NodoRadixTrie* NodoRadixTrie::obtenerNodo(const string& w) {
    // Caso base: w vacio
    if (w.empty()) {
        return this;
    }
    // Recorrer todos los hijos hasta encontrar w
    for (auto& par : hijos) {
        string palabraHijo = par.first;
        string prefijo = prefijoCom(palabraHijo, w);
        if (not prefijo.empty()) {
            // w = blanco, palabraHijo = bla, prefijo = bla
            if (prefijo == palabraHijo) {
                // Seguir recursivamente
                string resto = w.substr(prefijo.length());
                return par.second->obtenerNodo(resto);
            }
            // Para el autocompletado
            // w = bla, palabraHijo = blanco, prefijo = bla
            if (prefijo == w and prefijo.length() < palabraHijo.length()) {
                // Retornar el nodo hijo, pero ajustar la búsqueda
                return par.second;
            }
        }
    }
    return NULL;
}

void NodoRadixTrie::insertarPalabra(const string& w, int l, int p) {
    NodoRadixTrie* aux = this;  // empezamos desde la raiz
    if (w.empty()) {
        hoja = true;
        lineaPos.push_back({l, p});
        return;
    }
    for (auto par : hijos) {
        string palabraHijo = par.first;
        string prefijo = prefijoCom(palabraHijo, w);  // en comun
        if (not prefijo.empty()) {                    // Tienen prefijo en comun
            if (prefijo == palabraHijo) {             // El prefijo es un hijo
                /*
                    w = testing
                    palabraHijo = test
                    resto = ing
                */
                string resto = w.substr(palabraHijo.length());
                // Ponemos la nueva linea y posicion
                par.second->insertarPalabra(resto, l, p);
                return;
            }
            if (prefijo == w) {
                /*
                    w = blanco
                    palabraHijo = blancos
                    prefijo = blanco
                */
                NodoRadixTrie* nodo_viejo = par.second;
                NodoRadixTrie* nodo_nuevo = new NodoRadixTrie(true);
                nodo_nuevo->lineaPos.push_back({l, p});
                // Reconectar: prefijo → nuevo → resto → nodo_viejo
                string resto = palabraHijo.substr(prefijo.length());  // "s"
                nodo_nuevo->hijos[resto] = nodo_viejo;
                hijos.erase(par.first);
                hijos[prefijo] = nodo_nuevo;
                return;
            }
            if (prefijo.length() < palabraHijo.length() and
                prefijo.length() < w.length()) {
                /*
                    palabraHijo = blanco
                    w = blasfemia
                    prefijo = bla
                */
                NodoRadixTrie* nodo_intermedio = new NodoRadixTrie();
                NodoRadixTrie* nodo_viejo = par.second;
                string resto = palabraHijo.substr(prefijo.length());  // nco
                nodo_intermedio->hijos[resto] = nodo_viejo;
                string resto_w = w.substr(prefijo.length());  // sfemia
                NodoRadixTrie* nodo_nuevo = new NodoRadixTrie(true);
                nodo_nuevo->lineaPos.push_back({l, p});
                nodo_intermedio->hijos[resto_w] = nodo_nuevo;
                // unir con raiz
                hijos.erase(par.first);
                hijos[prefijo] = nodo_intermedio;
                return;
            }
        }
    }
    // No hay prefijo comun
    NodoRadixTrie* nodo = new NodoRadixTrie(true);
    nodo->lineaPos.push_back({l, p});
    hijos[w] = nodo;
}

vector<pair<int, int>> NodoRadixTrie::buscarPalabra(const string& w) {
    // Caso base: si w esta vacio y es hoja
    if (w.empty()) {
        if (hoja) {
            return lineaPos;
        }
        return vector<pair<int, int>>();
    }
    for (auto& par : hijos) {
        string palabraHijo = par.first;
        string prefijo = prefijoCom(palabraHijo, w);
        if (not prefijo.empty()) {
            // w = blanco, palabraHijo = bla, prefijo = bla
            if (prefijo == palabraHijo) {
                string resto = w.substr(prefijo.length());
                return par.second->buscarPalabra(resto);
            }
            // w = bla, palabraHijo = blanco, prefijo = bla
            if (prefijo == w) {
                return vector<pair<int, int>>();  // no encontrado
            }
        }
    }
    return vector<pair<int, int>>();  // no encontrado
}

vector<pair<int, int>> NodoRadixTrie::buscarFrase(const vector<string>& frase) {
    if (frase.empty()) return vector<pair<int, int>>();
    // guardo todas las ocurrencias de cada palabra de la frase
    vector<vector<pair<int, int>>> ocurrenciasPalabras(frase.size());
    for (int i = 0; i < frase.size(); ++i) {
        vector<pair<int, int>> occs = buscarPalabra(frase[i]);
        if (occs.empty()) return {};  // Si falta una palabra
        ocurrenciasPalabras[i] = occs;
    }
    vector<pair<int, int>> res;
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
            res.push_back({linea, posInicial});
        }
    }
    return res;
}

bool NodoRadixTrie::getHoja() const { return hoja; }

int NodoRadixTrie::getNodos() { return nodos; }

unordered_map<string, NodoRadixTrie*>& NodoRadixTrie::getHijos() {
    return hijos;
}

size_t NodoRadixTrie::calcularMemoria() const {
    size_t total = sizeof(NodoRadixTrie);
    // Memoria del vector lineaPos
    total += lineaPos.capacity() * sizeof(pair<int, int>);
    // Memoria del unordered_map (aproximado)
    total += hijos.bucket_count() * sizeof(void*);
    // Para cada entrada en el map
    for (const auto& par : hijos) {
        // Memoria de la string (key)
        total += sizeof(string);                       // objeto string
        total += par.first.capacity() * sizeof(char);  // contenido de la string
        // Memoria del puntero
        total += sizeof(NodoRadixTrie*);
        // Overhead del unordered_map por entrada
        total += 16;
        // Recursivo: memoria de los hijos
        total += par.second->calcularMemoria();
    }
    return total;
}