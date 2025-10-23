#include <algorithm>
#include <cctype>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>

#include "NodoRadixTrie.hh"
#include "NodoTrie.hh"

using namespace std;

const string SEPARADOR = "==================================";

string selectType() {
    cout << "Select the type of Trie to use:" << endl
         << endl
         << "[1] Normal Trie" << endl
         << "[2] Radix Trie" << endl
         << endl
         << "Enter an option: ";
    int m;
    cin >> m;
    if (m == 1) return "normal";
    if (m == 2) return "radix";
    cout << "Invalid option. Please enter 1 or 2." << endl;
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
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
    cout << SEPARADOR << endl;
    cout << "Select a file from the list below:" << endl;
    cout << endl;
    for (size_t i = 0; i < archivos.size(); ++i) {
        cout << "[" << i + 1 << "] " << archivos[i] << endl;
    }
    cout << "[0] Enter name manually" << endl;
    cout << endl;
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
        cout << "Enter an option: ";
        string input;
        cin >> input;
        int i;
        try {
            i = stoi(input);
        } catch (...) {  // generico
            cout << SEPARADOR << endl;
            cout << "Invalid input. Please enter a valid number." << endl;
            continue;
        }
        cout << SEPARADOR << endl;
        --i;
        if (i >= (int)lista.size())
            cout << "Error: index not available" << endl << endl;
        else if (i >= 0) {
            cout << "The file selected was '" << lista[i] << "'" << endl;
            cout << "Reading file '" << lista[i] << "'..." << endl;
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
    cout << "Available Commands:" << endl;
    cout << endl;
    cout << "[h] Show commands" << endl;
    cout << "[s] Search a word" << endl;
    cout << "[o] Open a new file" << endl;
    cout << "[x] Exit" << endl;
    cout << endl;
    cout << "Enter an option: ";
}

void imprimirVector(const vector<string>& v, const string& w) {
    if (not v.empty()) {
        cout << "Maybe you wanted to search: ";
        int count = 0;
        for (const string& it : v) {
            cout << it << "     ";
            ++count;
            if (count % 16 == 0) cout << endl;
        }
        cout << endl
             << endl
             << "Total words founds with the prefix " << w << ": " << count
             << endl;

        cout << SEPARADOR << endl;
    }
}

template <typename T>
void searchCommand(T& trie) {
    cout << "Enter the word you want to search: ";
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
    int nodosVisitados = 0;
    vector<pair<int, int>> lineaPos = trie.buscarFrase(frase, nodosVisitados);
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
        cout << SEPARADOR << endl;
        // Autocompletado
        if (frase.size() == 1) {
            int nodosAux = 0;
            // Se puede cambiar el metodo de busca

            // int n = 5;  // prefijos
            // vector<string> palabras = trie.completarPrefijo2(word, nodosAux,
            // n);
            vector<string> palabras = trie.completarPrefijo(word, nodosAux);

            nodosVisitados += nodosAux;
            imprimirVector(palabras, word);
        }
    } else {
        cout << "Couldn't find the word '" << word << "'" << endl;
        cout << SEPARADOR << endl;
        if (frase.size() == 1) {
            int nodosAux = 0;
            // Se puede cambiar el metodo de busca

            // int n = 5;  // prefijos
            // vector<string> palabras = trie.completarPrefijo2(word, nodosAux,
            // n);

            vector<string> palabras = trie.completarPrefijo(word, nodosAux);
            nodosVisitados += nodosAux;
            imprimirVector(palabras, word);
        }
    }
    // Mostrar tiempo en milisegundos
    cout << "Search took: " << duracion.count() << " ms" << endl;
    cout << "Visited nodes: " << nodosVisitados << endl;
    cout << SEPARADOR << endl;
}

template <typename T>
void openCommand(const vector<string>& lista, T& trie, bool& valido) {
    cout << "Open file:" << endl;
    trie = T();
    valido = false;
    readFile(valido, lista, trie);
    string tipo = "radix";
    if (typeid(trie) == typeid(NodoTrie)) {
        tipo = "normal";
    }
    mostrarEstadisticasTrie(trie, tipo);
}

template <typename T>
void searchApplication(const vector<string>& lista, T& trie, bool& valido) {
    char command;
    cin >> command;
    while (valido and command != 'x') {
        if (command == 's') {
            searchCommand(trie);
            cout << "Enter an option: ";
        } else if (command == 'o') {
            openCommand(lista, trie, valido);
            cout << "Enter an option: ";
        } else {
            show_commands();
        }
        cin >> command;
    }
}

template <typename T>
void mostrarEstadisticasTrie(const T& trie, const string& tipo) {
    cout << "File was read correctly." << endl;
    cout << SEPARADOR << endl;
    cout << "Total nodes created in the " << tipo
         << " Trie: " << trie.getNodos() << endl;
    size_t memoriaMB = trie.calcularMemoria() / (1024 * 1024);
    cout << "Memory used by the " << tipo << " Trie: " << memoriaMB << " MB"
         << endl;
    cout << "Average height of the " << tipo
         << " Trie: " << trie.calcularProfundidadMedia() << endl;
    cout << SEPARADOR << endl;
}

template <typename T>
void ejecutarBusqueda(const string& tipoNombre, const vector<string>& lista) {
    T trie;
    bool valido = false;
    readFile(valido, lista, trie);
    mostrarEstadisticasTrie(trie, tipoNombre);
    show_commands();
    searchApplication(lista, trie, valido);
}

int main() {
    string type = selectType();
    vector<string> lista = listarArchivosTxt();
    if (type == "normal") {
        ejecutarBusqueda<NodoTrie>("normal", lista);
    } else {
        ejecutarBusqueda<NodoRadixTrie>("radix", lista);
    }
}