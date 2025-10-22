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

#include "NodoRadixTrie.hh"
#include "NodoTrie.hh"

using namespace std;

string selectType() {
    cout << ">> Seleccione el tipo de trie:" << endl;
    cout << "   1. Normal trie" << endl;
    cout << "   2. Radix trie" << endl;
    int m;
    cin >> m;
    if (m == 1) return "normal";
    if (m == 2) return "radix";
    cout << "Tipo inválido" << endl;
    return selectType();
}

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

string limpiarPalabra(const string& s) {
    string res;
    for (char c : s) {
        if (isalpha((unsigned char)c))  // solo letras
            res += tolower((unsigned char)c);
    }
    return res;
}

template <typename T>
bool leerEntrada(const string& nombreArchivo, T& t) {
    ifstream archivo(nombreArchivo);
    string linea;
    int numeroLinea = 1;
    if (not archivo.is_open()) {
        cout << "Error: The file '" << nombreArchivo << "' could not be opened"
             << endl;
        return false;
    }
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string word;
        int pos = 0;
        while (ss >> word) {
            // Normalizar la palabra
            word = limpiarPalabra(word);
            if (not word.empty()) {
                t.insertarPalabra(word, numeroLinea, pos);
            }
            ++pos;
        }
        ++numeroLinea;
    }
    return true;
}

template <typename T>
void readFile(bool& valido, const vector<string>& lista, T& trie) {
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
         << "   x: exit" << endl;
}

template <typename T>
void searchCommand(T& trie) {
    cout << ">> Search word: ";
    string words;
    cin.ignore();
    getline(cin, words);
    stringstream ss(words);
    string word;
    vector<string> frase;
    while (ss >> word) {
        word = limpiarPalabra(word);
        if (not word.empty()) {
            frase.push_back(word);
        }
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
                for (const string& it : palabras) {
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
                for (const string& it : palabras) {
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

template <typename T>
void openCommand(const vector<string>& lista, T& trie, bool& valido) {
    cout << "Open file:" << endl;
    trie = T();
    valido = false;
    readFile(valido, lista, trie);
    cout << "=============================" << endl;
    cout << "File was read correctly." << endl;
}

template <typename T>
void searchApplication(const vector<string>& lista, T& trie, bool& valido) {
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
    string type = selectType();
    bool valido = false;
    vector<string> lista = listarArchivosTxt();
    if (type == "normal") {
        NodoTrie trie;
        readFile(valido, lista, trie);
        cout << "=============================" << endl;
        cout << "File was read correctly." << endl;
        cout << "Total nodes created in Trie: " << trie.getNodos() << endl;
        size_t memoriaMB = trie.calcularMemoria() / (1024 * 1024);
        cout << "Memoria del Trie: " << memoriaMB << " MB" << endl;
        show_commands();
        searchApplication(lista, trie, valido);
    } else {
        NodoRadixTrie trie;
        readFile(valido, lista, trie);
        cout << "=============================" << endl;
        cout << "File was read correctly." << endl;
        cout << "Total nodes created in Trie: " << trie.getNodos() << endl;
        size_t memoriaMB = trie.calcularMemoria() / (1024 * 1024);
        cout << "Memoria del Trie: " << memoriaMB << " MB" << endl;
        show_commands();
        searchApplication(lista, trie, valido);
    }
}