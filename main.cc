#include <iostream>
#include "NodoTrie.hh"
#include <string>
#include <fstream>
#include <sstream>
#include <list>
#include <limits>

using namespace std;

bool leerEntrada(const string &nombreArchivo, NodoTrie &t) {
    ifstream archivo(nombreArchivo);
    string linea;
    int numeroLinea = 1;
    
    if (!archivo.is_open()) {
        cout << "Error: No se pudo abrir el archivo" << endl;
        return false;
    }

    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string word;
        while (ss >> word) {
            t.insertar_word(word, numeroLinea);
        }
        ++numeroLinea;
    }
    return true;
}


int main() {
    char c;
    NodoTrie trie;
    cout << ">>File name: ";
    string archivo;
    cin >> archivo;
    bool valido = leerEntrada(archivo, trie);
    cout << "===================" << endl;
    cout << "File was read." << endl;
    cout << ">>Press x to exit or press f to search a word: ";
    char command;
    cin >> command;
    while (valido and command != 'x') {
        if (command == 'f') {
            cout << ">>Search word: ";
            string word;
            cin >> word;
            list<int> lines = trie.buscar_word(word);
            if (lines.empty()) {
                cout << "Couldn't find the word." << endl;
            }
            else {
                cout << "Word found in lines: ";
                int count = 0;
                for (int it : lines) {
                    cout << it << "     ";
                    ++count;
                    if (count % 16 == 0) cout << endl;
                }
                cout << endl << "Total words founds: " << count << endl;
            }
        }

        cout << ">>Press x to exit or press f to search a word: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin >> command;
    }
}