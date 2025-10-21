#include "NodoRadixTrie.hh"

// Constructor
NodoRadixTrie::NodoRadixTrie() { hoja = false; }

NodoRadixTrie::NodoRadixTrie(bool h) { hoja = h; }

NodoRadixTrie::~NodoRadixTrie() {
    for (auto& par : hijos) {
        delete par.second;
    }
    hijos.clear();
}

string NodoRadixTrie::prefijos(string s1, string s2) {
    int i = 0, minLong = min(s1.length(), s2.length());
    while (i < minLong && s1[i] == s2[i]) {
        ++i;
    }
    return s1.substr(0, i);
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
    aux->lineas.push_back(l);
}

list<int> NodoTrie::buscar_word(string w) {
    NodoTrie* word = obtener(w);
    if (word != NULL && word->hoja) {
        return word->lineas;
    }
    return list<int>();  // lista vacía si no se encuentra
}

bool NodoTrie::esHoja() const { return hoja; }

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
