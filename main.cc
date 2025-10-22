#include <algorithm>
#include <cctype>
#include <chrono>
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
        int pos = 0;
        while (ss >> word) {
            std::transform(word.begin(), word.end(), word.begin(),
                           [](unsigned char c) { return std::tolower(c); });

            t.insertar_word(word, numeroLinea, pos);
            ++pos;
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
        if (i >= (int)lista.size())
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
    cout << "   h: show commands  " << endl
         << "   s: search a word  " << endl
         << "   o: open new file  " << endl
         << "   t: use trie       " << endl
         << "   r: use radix trie " << endl
         << "   x: exit" << endl;
}

void searchCommand(NodoTrie& trie) {
    cout << ">> Search word: ";
    string words;
    cin.ignore();
    getline(cin, words);
    stringstream ss(words);
    string word;
    vector<string> frase;
    while (ss >> word) {
        frase.push_back(word);
    }

    // Medición del tiempo
    auto inicio = chrono::high_resolution_clock::now();
    vector<pair<int, int>> lineaPos = trie.buscarFrase(frase);
    auto fin = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duracion = fin - inicio;

    if (not lineaPos.empty()) {
        cout << "Word found in lines: ";
        int count = 0;
        for (int i = 0; i < lineaPos.size(); ++i) {
            cout << lineaPos[i].first << "     ";
            ++count;
            if (count % 16 == 0) cout << endl;
        }
        cout << endl << endl << "Total words founds: " << count << endl;
        cout << "===================" << endl;

        if (frase.size() == 1) {
            vector<string> palabras = trie.completarPrefijo(word);
            if (not palabras.empty()) {
                cout << "Maybe you wanted to search: ";
                int count = 0;
                for (string it : palabras) {
                    cout << it << "     ";
                    ++count;
                    if (count % 16 == 0) cout << endl;
                }
                cout << endl
                     << endl
                     << "Total words founds with the prefix " << word << ": "
                     << count << endl;

                cout << "===================" << endl;
            }
        }
    } else {
        cout << "Couldn't find the word '" << word << "'" << endl;
        cout << "===================" << endl;
        if (frase.size() == 1) {
            vector<string> palabras = trie.completarPrefijo(word);
            if (not palabras.empty()) {
                cout << "Maybe you wanted to search: ";
                int count = 0;
                for (string it : palabras) {
                    cout << it << "     ";
                    ++count;
                    if (count % 16 == 0) cout << endl;
                }
                cout << endl
                     << endl
                     << "Total words founds with the prefix " << word << ": "
                     << count << endl;

                cout << "===================" << endl;
            }
        }
    }
    // Mostrar tiempo en milisegundos
    cout << "Search took: " << duracion.count() << " ms" << endl;
    cout << "===================" << endl;
}

void openCommand(const vector<string>& lista, NodoTrie& trie, bool& valido) {
    cout << "Open file:" << endl;
    trie.~NodoTrie();
    valido = false;
    readFile(valido, lista, trie);
    cout << "=============================" << endl;
    cout << "File was read correctly." << endl;
}

void searchApplication(const vector<string>& lista, NodoTrie& trie,
                       bool& valido) {
    char command;
    cin >> command;
    while (valido and command != 'x') {
        if (command == 's') {
            searchCommand(trie);
        } else if (command == 'o') {
            openCommand(lista, trie, valido);
        } else if (command == 'h') {
            show_commands();
        }
        cout << "enter the command: ";
        cin >> command;
    }
}

int main() {
    NodoTrie trie;
    bool valido = false;
    vector<string> lista = listarArchivosTxt();
    readFile(valido, lista, trie);
    cout << "=============================" << endl;
    cout << "File was read correctly." << endl;
    cout << "Total nodes created in Trie: " << trie.getNodos() << endl;
    size_t memoriaMB = trie.calcularMemoria() / (1024 * 1024);
    cout << "Memoria del Trie: " << memoriaMB << " MB" << endl;
    show_commands();
    searchApplication(lista, trie, valido);
}