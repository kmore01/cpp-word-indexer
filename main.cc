#include <iostream>
#include "NodoTrie.hh"
#include <string>
#include <fstream>
#include <sstream>
#include <list>

using namespace std;

void leerEntrada(const string &nombreArchivo, NodoTrie &t) {
    ifstream archivo(nombreArchivo);
    string linea;
    int numeroLinea = 1;
    
    if (!archivo.is_open()) {
        cout << "Error: No se pudo abrir el archivo" << endl;
    }

    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string word;
        while (ss >> word) {
            t.insertar_word(word, numeroLinea);
        }
        ++numeroLinea;
    }
    

}


int main() {
    char c;
    NodoTrie trie;
    cout << "File name: ";
    string archivo;
    cin >> archivo;
    leerEntrada(archivo, trie);
    cout << "File was read" << endl;
    char command;
    cin >> command;
    while (command != 'x') {
        if (command == 'f') {
            cout << "Search word: ";
            string word;
            cin >> word;
            list<int> lines = trie.buscar_word(word);
            if (lines.empty()) {
                cout << "Couldn't find the word." << endl;
            }
            else {
                cout << "Word find in the lines: ";
                for (int it : lines) {
                    cout << it << " " << endl;
                }
            }
        }

        cout << "Press x to exit or press f to search a word";
        cin >> command;
    }
}