#include <iostream>
#include "NodoTrie.hh"
#include <string>

using namespace std;

int main() {
    char c;
    NodoTrie trie;
    while(cin >> c) {
        NodoTrie actual = &trie;
        if (c != ' ') {
            actual.insertar(c);
        }
    }
}