#include "NodoTrie.hh"

// Constructor
NodoTrie::NodoTrie() {
    hoja = false;
}

NodoTrie::NodoTrie(bool h) {
    hoja = h;
}

NodoTrie::~NodoTrie() {
    for (auto &par : hijos) {
        delete par.second; 
    }
    hijos.clear();
}

// Insertar nuevo nodo hijo
void NodoTrie::insertar(char c) { 
    hoja = false;
    hijos[c] = new NodoTrie(true); 
}

bool NodoTrie::esHoja() const {
    return hoja;
}

NodoTrie* NodoTrie::obtener(char c) const {
    auto it = hijos.find(c);
    if (it != hijos.end()) {
        return it->second;
    }
    return NULL;
}

unordered_map<char, NodoTrie*>& NodoTrie::getHijos() {
    return hijos;
}
