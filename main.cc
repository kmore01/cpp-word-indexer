#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <vector>

#include "NodoTrie.hh"

using namespace std;

vector<string> listarArchivosTxt(const string& directorio = ".") {
    vector<string> archivos;

    try {
        for (const auto& entrada : filesystem::directory_iterator(directorio)) {
            if (entrada.is_regular_file()) {
                string extension = entrada.path().extension().string();
                if (extension == ".txt" || extension == ".TXT") {
                    archivos.push_back(entrada.path().filename().string());
                }
            }
        }
    } catch (const filesystem::filesystem_error& e) {
        cout << "Error al leer el directorio: " << e.what() << endl;
    }

    return archivos;
}

void mostrarMenuArchivos(const vector<string>& archivos) {
    cout << "=== AVAILABLE FILES ===" << endl;
    for (size_t i = 0; i < archivos.size(); ++i) {
        cout << i + 1 << ". " << archivos[i] << endl;
    }
    cout << "0. Enter name manually" << endl;
    cout << "=============================" << endl;
}

bool leerEntrada(const string& nombreArchivo, NodoTrie& t) {
    ifstream archivo(nombreArchivo);
    string linea;
    int numeroLinea = 1;

    if (!archivo.is_open()) {
        cout << "Error: The file '" << nombreArchivo << "' could not be opened"
             << endl;
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

void readFile(bool& valido, const vector<string>& lista, NodoTrie& trie) {
    while (not valido) {
        mostrarMenuArchivos(lista);
        cout << ">> Select a file (number): ";
        int i;
        cin >> i;
        --i;
        if (i >= lista.size())
            cout << "error: index not available" << endl << endl;
        else if (i >= 0) {
            cout << "the file selected was '" << lista[i] << "'" << endl;
            cout << "reading file '" << lista[i] << "'..." << endl;
            valido = leerEntrada(lista[i], trie);
        } else {
            cout << ">> File name: ";
            string archivo;
            cin >> archivo;
            cout << "reading file '" << archivo << "'..." << endl;
            valido = leerEntrada(archivo, trie);
        }
    }
}

void show_commands() {
    cout << ">> Commands: " << endl;
    cout << "   h: show commands" << endl
         << "   s: search a word" << endl
         << "   o: open new file" << endl
         << "   x: exit" << endl;
}

int main() {
    char c;
    NodoTrie trie;
    bool valido = false;
    vector<string> lista = listarArchivosTxt();
    readFile(valido, lista, trie);
    cout << "=============================" << endl;
    cout << "File was read correctly." << endl;
    // cout << ">>Press x to exit or press f to search a word: ";
    show_commands();
    char command;
    cin >> command;
    while (valido and command != 'x') {
        if (command == 's') {
            cout << ">> Search word: ";
            string word;
            cin >> word;

            if (trie.contienePalabra(word)) {
                list<int> lines = trie.buscar_word(word);
                if (lines.empty()) {
                    cout << "Couldn't find the word '" << word << "'" << endl;
                } else {
                    cout << "Word found in lines: ";
                    int count = 0;
                    for (int it : lines) {
                        cout << it << "     ";
                        ++count;
                        if (count % 16 == 0) cout << endl;
                    }
                    cout << endl
                         << endl
                         << "Total words founds: " << count << endl;
                }
                cout << "===================" << endl;
            }

            else {
                list<string> palabras = trie.completarPrefijo(word);
                if (palabras.empty()) {
                    cout << "Couldn't find the word '" << word << "'" << endl;
                } else {
                    cout << "Maybe you wanted to search: ";
                    int count = 0;
                    for (string it : palabras) {
                        cout << it << "     ";
                        ++count;
                        if (count % 16 == 0) cout << endl;
                    }
                    cout << endl
                         << endl
                         << "Total words founds with the prefix " << word
                         << ": " << count << endl;
                }
                cout << "===================" << endl;
            }

        } else if (command == 'o') {
            cout << "Open file:" << endl;
            NodoTrie aux;
            trie = aux;
            valido = false;
            readFile(valido, lista, trie);
            cout << "=============================" << endl;
            cout << "File was read correctly." << endl;
        } else if (command == 'h')
            show_commands();
        cout << "enter the command: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin >> command;
    }
}