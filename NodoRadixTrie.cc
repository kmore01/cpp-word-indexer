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

string NodoRadixTrie::prefijoCom(string s1, string s2) {
    int i = 0, minLong = min(s1.length(), s2.length());
    while (i < minLong && s1[i] == s2[i]) {
        ++i;
    }
    return s1.substr(0, i);
}

NodoRadixTrie* NodoRadixTrie::obtener(string w) {
    // Caso base: si w está vacío, devolvemos este nodo
    if (w.empty()) {
        return this;
    }

    // Buscar entre los hijos
    for (auto& par : hijos) {
        string palabraHijo = par.first;
        string prefijo = prefijoCom(palabraHijo, w);

        if (not prefijo.empty()) {
            // Si el prefijo coincide exactamente con la palabra del hijo
            if (prefijo == palabraHijo) {
                // Calcular el resto de w y seguir recursivamente
                string resto = w.substr(prefijo.length());
                return par.second->obtener(resto);
            }

            // Si el prefijo es igual a w pero menor que palabraHijo
            // ESTE ES EL CASO CRÍTICO PARA AUTOCOMPLETAR
            // w = "bla", palabraHijo = "blanco"
            // Debes retornar el nodo hijo porque desde ahí se autocompleta
            if (prefijo == w && prefijo.length() < palabraHijo.length()) {
                // Retornar el nodo hijo, pero ajustar la búsqueda
                return par.second;
            }
        }
    }

    // No se encontró ningún hijo con prefijo común
    return nullptr;
}

void NodoRadixTrie::insertar_word(string w, int l, int p) {
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
                par.second->insertar_word(resto, l, p);
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

vector<pair<int, int>> NodoRadixTrie::buscar_word(string w) {
    // Caso base: si w está vacío y este nodo es hoja
    if (w.empty()) {
        if (hoja) {
            return lineaPos;
        }
        return vector<pair<int, int>>();
    }

    for (auto par : hijos) {
        string palabraHijo = par.first;
        string prefijo = prefijoCom(palabraHijo, w);

        if (not prefijo.empty()) {
            // Si el prefijo es igual a la palabra del hijo
            if (prefijo == palabraHijo) {
                // Calcular el resto de W, no de palabraHijo
                string resto = w.substr(prefijo.length());
                return par.second->buscar_word(resto);
            }
            // Si el prefijo es igual a w pero no coincide completo con
            // palabraHijo significa que w no está en el trie
            if (prefijo == w) {
                return vector<pair<int, int>>();
            }
        }
    }
    return vector<pair<int, int>>();  // no encontrado
}

bool NodoRadixTrie::esHoja() const { return hoja; }

int NodoRadixTrie::getNodos() { return nodos; }

unordered_map<string, NodoRadixTrie*>& NodoRadixTrie::getHijos() {
    return hijos;
}

vector<pair<int, int>> NodoRadixTrie::buscarFrase(const vector<string>& frase) {
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

vector<string> NodoRadixTrie::completarPrefijo(const string& w) {
    vector<string> resultados;

    // Buscar el nodo y el camino hasta él
    string prefijoActual = "";
    NodoRadixTrie* nodo = this;
    string resto = w;

    while (!resto.empty() && nodo != nullptr) {
        bool encontrado = false;

        for (auto& par : nodo->hijos) {
            string palabraHijo = par.first;
            string prefijo = prefijoCom(palabraHijo, resto);

            if (!prefijo.empty()) {
                // Si el prefijo coincide exactamente con palabraHijo
                if (prefijo == palabraHijo) {
                    prefijoActual += palabraHijo;
                    resto = resto.substr(prefijo.length());
                    nodo = par.second;
                    encontrado = true;
                    break;
                }
                // Si encontramos que 'w' termina en medio de una arista
                else if (prefijo == resto) {
                    // Añadir toda la arista al prefijo
                    prefijoActual += palabraHijo;
                    nodo = par.second;
                    resto = "";
                    encontrado = true;
                    break;
                }
            }
        }

        if (!encontrado) {
            return resultados;  // No se encontró el prefijo
        }
    }

    // Ahora hacer DFS desde este nodo
    if (nodo != nullptr) {
        completarDFS(nodo, prefijoActual, resultados);
    }

    return resultados;
}

void NodoRadixTrie::completarDFS(NodoRadixTrie* nodo, string& palabraActual,
                                 vector<string>& resultados) {
    if (nodo->hoja) {
        resultados.push_back(palabraActual);
    }

    for (auto& [sufijo, hijo] : nodo->hijos) {
        // Guardar el tamaño original para hacer backtrack correctamente
        size_t tamañoOriginal = palabraActual.size();

        // Añadir el sufijo completo (no solo un carácter)
        palabraActual += sufijo;

        completarDFS(hijo, palabraActual, resultados);

        // Backtrack: restaurar al tamaño original
        palabraActual.resize(tamañoOriginal);
    }
}

size_t NodoRadixTrie::calcularMemoria() const {
    size_t total = sizeof(NodoRadixTrie);  // tamaño base del nodo

    // Memoria del vector lineaPos
    total += lineaPos.capacity() * sizeof(pair<int, int>);

    // Memoria del unordered_map (aproximado)
    total += hijos.bucket_count() * sizeof(void*);  // buckets

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