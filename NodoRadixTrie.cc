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
    while (i < minLong and s1[i] == s2[i]) {
        ++i;
    }
    return s1.substr(0, i);
}

vector<string> NodoRadixTrie::contarPalabrasPrefijo(const string& w, int& nv) {
    vector<string> res;
    nv = 0;
    completarPrefijoAux(w, "", res, nv);
    return res;
}

int NodoRadixTrie::partition(vector<pair<string, int>>& vec, int left,
                             int right) {
    int pivotValue = vec[right].second;
    int idx = left;
    for (int j = left; j < right; j++) {
        if (vec[j].second >= pivotValue) {  // Orden descendente
            swap(vec[idx], vec[j]);
            idx++;
        }
    }
    swap(vec[idx], vec[right]);
    return idx;
}

pair<string, int> NodoRadixTrie::encontrarIesimoMayor(
    vector<pair<string, int>>& vec, int i) {
    if (i < 0 or i >= vec.size()) {
        return {};
    }
    int left = 0, right = vec.size() - 1;
    while (left <= right) {
        int pivotIndex = partition(vec, left, right);

        if (pivotIndex == i) {
            return vec[i];  // Encontramos el i-ésimo mayor
        } else if (pivotIndex < i) {
            left = pivotIndex + 1;  // Buscar en la derecha
        } else {
            right = pivotIndex - 1;  // Buscar en la izquierda
        }
    }
    return vec[i];
}

vector<string> NodoRadixTrie::obtenerTop5MasFrecuentes(const string& w, int& nv) {
    // Siempre retornar las 5 más frecuentes
    int n = 5;
    vector<pair<string, int>> res;
    vector<string> ret(n);
    nv = 0;
    completarPrefijoAux2(w, "", res, nv);
    for (int i = 0; i < n; ++i) {
        string aux = encontrarIesimoMayor(res, i).first;
        ret[i] = aux;
    }
    return ret;
}

void NodoRadixTrie::completarPrefijoAux2(const string& prefijoBuscado,
                                         string palabraActual,
                                         vector<pair<string, int>>& resultados,
                                         int& nv) {
    ++nv;
    // Si ya consumimos todo el prefijo, recolectar todas las palabras
    if (prefijoBuscado.empty()) {
        if (hoja) {
            resultados.push_back({palabraActual, lineaPos.size()});
        }
        for (auto& par : hijos) {  //
            string nuevaPalabra = palabraActual + par.first;
            par.second->completarPrefijoAux2(
                "", nuevaPalabra, resultados,
                nv);  // string vacio porque ya tenemos el prefijo comun
        }
        return;
    }
    for (auto& par : hijos) {
        string palabraHijo = par.first;
        string prefijo = prefijoCom(palabraHijo, prefijoBuscado);
        if (not prefijo.empty()) {
            if (prefijo == palabraHijo) {
                // El edge completo coincide, seguir buscando
                string resto = prefijoBuscado.substr(prefijo.length());
                par.second->completarPrefijoAux2(
                    resto, palabraActual + palabraHijo, resultados, nv);
            } else if (prefijo == prefijoBuscado) {
                // El prefijo buscado está contenido en este edge
                // Recolectar todo desde aquí
                string nuevaPalabra = palabraActual + palabraHijo;
                par.second->completarPrefijoAux2("", nuevaPalabra, resultados,
                                                 nv);
            }
        }
    }
}

void NodoRadixTrie::completarPrefijoAux(const string& prefijoBuscado,
                                        string palabraActual,
                                        vector<string>& resultados, int& nv) {
    if (resultados.size() < 5) {
        ++nv;
        // Si ya consumimos todo el prefijo, recolectar todas las palabras
        if (prefijoBuscado.empty()) {
            if (hoja) {
                resultados.push_back(palabraActual);
            }
            for (auto& par : hijos) {
                string nuevaPalabra = palabraActual + par.first;
                par.second->completarPrefijoAux("", nuevaPalabra, resultados,
                                                nv);
            }
            return;
        }
        for (auto& par : hijos) {
            string palabraHijo = par.first;
            string prefijo = prefijoCom(palabraHijo, prefijoBuscado);
            if (not prefijo.empty()) {
                if (prefijo == palabraHijo) {
                    // El edge completo coincide, seguir buscando
                    string resto = prefijoBuscado.substr(prefijo.length());
                    par.second->completarPrefijoAux(
                        resto, palabraActual + palabraHijo, resultados, nv);
                } else if (prefijo == prefijoBuscado) {
                    // El prefijo buscado está contenido en este edge
                    // Recolectar todo desde aquí
                    string nuevaPalabra = palabraActual + palabraHijo;
                    par.second->completarPrefijoAux("", nuevaPalabra,
                                                    resultados, nv);
                }
            }
        }
    }
}

void NodoRadixTrie::completarDFS(NodoRadixTrie* nodo, string& palabraActual,
                                 vector<string>& resultados, int& nv) {
    ++nv;
    // Caso base: el nodo es hoja
    if (nodo->hoja) {
        resultados.push_back(palabraActual);
    }
    for (auto& par : nodo->hijos) {
        size_t tamañoOriginal = palabraActual.size();
        // Ponerle el sufijo completo
        palabraActual += par.first;
        completarDFS(par.second, palabraActual, resultados, nv);
        // Backtracking
        palabraActual.resize(tamañoOriginal);
    }
}

NodoRadixTrie* NodoRadixTrie::obtenerNodo(const string& w, int& nv) {
    ++nv;
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
                return par.second->obtenerNodo(resto, nv);
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
    for (auto& par : hijos) {
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

vector<pair<int, int>> NodoRadixTrie::buscarPalabra(const string& w, int& nv) {
    //  Caso base: si w esta vacio y es hoja
    if (w.empty()) {
        ++nv;
        if (hoja) {
            return lineaPos;
        }
        return vector<pair<int, int>>();
    }
    ++nv;
    for (auto& par : hijos) {
        string palabraHijo = par.first;
        string prefijo = prefijoCom(palabraHijo, w);
        if (not prefijo.empty()) {
            // w = blanco, palabraHijo = bla, prefijo = bla
            if (prefijo == palabraHijo) {
                string resto = w.substr(prefijo.length());
                int nvHijo = 0;
                vector<pair<int, int>> res =
                    par.second->buscarPalabra(resto, nvHijo);
                nv += nvHijo;
                return res;
            }
            // w = bla, palabraHijo = blanco, prefijo = bla
            if (prefijo == w) {
                return vector<pair<int, int>>();  // no encontrado
            }
        }
    }
    return vector<pair<int, int>>();  // no encontrado
}

vector<pair<int, int>> NodoRadixTrie::buscarFrase(const vector<string>& frase,
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

void NodoRadixTrie::calcularProfundidadesAux(int profundidadActual,
                                             int& contador,
                                             long long& suma) const {
    // Si este nodo es hoja (final de palabra)
    if (hoja) {
        ++contador;
        suma += profundidadActual;
    }
    // Recorrer todos los hijos
    for (const auto& par : hijos) {
        if (par.second != nullptr) {
            // Contar 1 por cada arista, NO la longitud del string
            par.second->calcularProfundidadesAux(
                profundidadActual + 1,  // Cambiar de +longitudArista a +1
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
        maxProf = max(maxProf, profundidadActual);
    }
    for (const auto& par : hijos) {
        if (par.second != nullptr) {
            par.second->calcularProfundidadMaximaAux(
                profundidadActual + 1,  // Cambiar de +longitudArista a +1
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