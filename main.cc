#include <iostream>
#include "NodoTrie.hh"
#include <string>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <filesystem>
#include <stdexcept>

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

bool leerEntrada(const string &nombreArchivo, NodoTrie &t) {
    ifstream archivo(nombreArchivo);
    
    if (!archivo.is_open()) {
        cout << "Error: The file '" << nombreArchivo << "' could not be opened" << endl;
        return false;
    }

    string linea;
    int numeroLinea = 1;

    try {
        while (getline(archivo, linea)) {
            stringstream ss(linea);
            string word;
            while (ss >> word) {
                t.insertar_word(word, numeroLinea);
            }
            ++numeroLinea;
        }
        archivo.close();    // to avoid possible problems
        return true;
    }
    catch (const exception& e) {
        cout << "Error reading file: " << e.what() << endl;
        archivo.close(); 
        return false;
    }
}

bool esNumeroStoi(const string& str) {
    if (str.empty()) return false;
    
    try {
        size_t pos = 0;
        int numero = stoi(str, &pos);
        
        // Verificar que se procesó todo el string
        return (pos == str.length());
    }
    catch (const invalid_argument& e) {
        return false;
    }
    catch (const out_of_range& e) {
        return false; // Número demasiado grande
    }
}

void readFile (bool &valido, const vector<string> &lista, NodoTrie &trie) {
    while (not valido) {
        mostrarMenuArchivos(lista);
        cout << ">> Select a file (number): ";
        string num;
        cin >> num; 
        if (esNumeroStoi(num)) {
            int i = stoi(num);
            --i;
            if (i >= 0) {
                if (i >= lista.size()) cout << "error: index not available" << endl << endl;
                else {
                    cout << "the file selected was '" << lista[i] << "'" << endl;
                    cout << "reading file '" << lista[i] << "'..." << endl;
                    valido = leerEntrada(lista[i], trie);
                }
            }
            else {
                cout << ">> File name: ";
                string archivo;
                cin >> archivo;
                cout << "reading file '" << archivo << "'..." << endl;
                valido = leerEntrada(archivo, trie);
            }
        }
        else {
            cout << "Error: It expected a number" << endl << "Press any key to continue" << endl;
            string aux;
            cin >> aux;
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
            list<int> lines = trie.buscar_word(word);
            if (lines.empty()) {
                cout << "Couldn't find the word '" << word << "'" << endl;
            }
            else {
                cout << "Word found in lines: ";
                int count = 0;
                for (int it : lines) {
                    cout << it << "     ";
                    ++count;
                    if (count % 16 == 0) cout << endl;
                }
                cout << endl << endl << "Total words founds: " << count << endl;
            }
            cout << "===================" << endl;
        }
        else if (command == 'o') {
            cout << "Open file:" << endl;
            NodoTrie aux;
            trie = aux;
            valido = false;
            readFile(valido, lista, trie);
                cout << "=============================" << endl;
                cout << "File was read correctly." << endl;
        }
        else if (command == 'h') show_commands();
        cout << "enter the command: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin >> command;
    }
}