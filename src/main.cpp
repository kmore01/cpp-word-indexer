#include <cctype>
#include <charconv>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include "trie/NodoRadixTrie.hpp"
#include "trie/NodoTrie.hpp"

namespace {

constexpr const char* SEPARADOR = "==================================";

// Pregunta al usuario qué implementación de trie quiere utilizar. Devuelve
// una cadena vacía si la entrada se cierra (EOF).
std::string selectType() {
    while (true) {
        std::cout << "Select the type of Trie to use:" << std::endl
                  << std::endl
                  << "[1] Normal Trie" << std::endl
                  << "[2] Radix Trie" << std::endl
                  << std::endl
                  << "Enter an option: ";
        int m = 0;
        if (!(std::cin >> m)) {
            if (std::cin.eof()) {
                std::cout << std::endl
                          << "Input closed. Exiting." << std::endl;
                return {};
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid option. Please enter 1 or 2." << std::endl;
            continue;
        }
        if (m == 1) return "normal";
        if (m == 2) return "radix";
        std::cout << "Invalid option. Please enter 1 or 2." << std::endl;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// Pregunta cómo debe resolverse el autocompletado. Devuelve -1 si la entrada
// se cierra (EOF).
int selectPrefixAlgorithm() {
    while (true) {
        std::cout << "Select the autocomplete preference:" << std::endl
                  << std::endl
                  << "[1] First found" << std::endl
                  << "[2] Most frequent" << std::endl
                  << std::endl
                  << "Enter an option: ";
        int m = 0;
        if (!(std::cin >> m)) {
            if (std::cin.eof()) {
                std::cout << std::endl
                          << "Input closed. Exiting." << std::endl;
                return -1;
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid option. Please enter 1 or 2." << std::endl;
            continue;
        }
        if (m == 1 || m == 2) return m;
        std::cout << "Invalid option. Please enter 1 or 2." << std::endl;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// Lee un entero validando que se consuma toda la cadena.
bool parseNumero(const std::string& texto, int& valor) {
    const char* inicio = texto.data();
    const char* fin = inicio + texto.size();
    const auto resultado = std::from_chars(inicio, fin, valor);
    return resultado.ec == std::errc() && resultado.ptr == fin;
}

// Lista los ficheros .txt del directorio indicado (por defecto, el actual).
std::vector<std::string> listarArchivosTxt(
    const std::string& directorio = ".") {
    std::vector<std::string> archivos;
    try {
        for (const auto& entrada :
             std::filesystem::directory_iterator(directorio)) {
            if (entrada.is_regular_file()) {
                const std::string extension =
                    entrada.path().extension().string();
                if (extension == ".txt" || extension == ".TXT") {
                    archivos.push_back(
                        entrada.path().filename().string());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cout << "Error al leer el directorio: " << e.what() << std::endl;
    }
    return archivos;
}

void mostrarMenuArchivos(const std::vector<std::string>& archivos) {
    std::cout << SEPARADOR << std::endl;
    std::cout << "Select a file from the list below:" << std::endl;
    std::cout << std::endl;
    for (std::size_t i = 0; i < archivos.size(); ++i) {
        std::cout << "[" << i + 1 << "] " << archivos[i] << std::endl;
    }
    std::cout << "[0] Enter name manually" << std::endl;
    std::cout << std::endl;
}

// Normaliza una palabra: conserva solo letras y las pasa a minúscula.
std::string limpiarPalabra(const std::string& s) {
    std::string res;
    for (char c : s) {
        if (std::isalpha(static_cast<unsigned char>(c))) {
            res += static_cast<char>(
                std::tolower(static_cast<unsigned char>(c)));
        }
    }
    return res;
}

// Lee el fichero línea a línea y registra cada palabra con su posición.
template <typename T>
bool leerEntrada(const std::string& nombreArchivo, T& t) {
    std::ifstream archivo(nombreArchivo);
    std::string linea;
    int numeroLinea = 1;
    if (!archivo.is_open()) {
        std::cout << "Error: The file '" << nombreArchivo
                  << "' could not be opened" << std::endl;
        return false;
    }
    while (std::getline(archivo, linea)) {
        std::stringstream ss(linea);
        std::string word;
        int pos = 0;
        while (ss >> word) {
            word = limpiarPalabra(word);
            if (!word.empty()) {
                t.insertarPalabra(word, numeroLinea, pos);
            }
            ++pos;
        }
        ++numeroLinea;
    }
    if (archivo.bad()) {
        std::cout << "Error reading the file '" << nombreArchivo << "'"
                  << std::endl;
        return false;
    }
    return true;
}

// Muestra el menú de ficheros hasta que se consigue leer uno válido. Si la
// entrada se cierra, deja `valido` a false y el programa termina.
template <typename T>
void readFile(bool& valido, const std::vector<std::string>& lista, T& trie) {
    while (!valido) {
        mostrarMenuArchivos(lista);
        std::cout << "Enter an option: ";
        std::string input;
        if (!(std::cin >> input)) {
            std::cout << std::endl << "Input closed. Exiting." << std::endl;
            return;
        }

        int seleccion = 0;
        if (!parseNumero(input, seleccion)) {
            std::cout << SEPARADOR << std::endl;
            std::cout << "Invalid input. Please enter a valid number."
                      << std::endl;
            continue;
        }
        std::cout << SEPARADOR << std::endl;

        if (seleccion == 0) {
            std::cout << ">> File name: ";
            std::string archivo;
            if (!(std::cin >> archivo)) {
                std::cout << std::endl
                          << "Input closed. Exiting." << std::endl;
                return;
            }
            std::cout << "reading file '" << archivo << "'..." << std::endl;
            valido = leerEntrada(archivo, trie);
            if (!valido) trie.clear();
        } else if (seleccion > 0 &&
                   static_cast<std::size_t>(seleccion) <= lista.size()) {
            const std::string& archivo =
                lista[static_cast<std::size_t>(seleccion) - 1];
            std::cout << "The file selected was '" << archivo << "'"
                      << std::endl;
            std::cout << "Reading file '" << archivo << "'..." << std::endl;
            valido = leerEntrada(archivo, trie);
            if (!valido) trie.clear();
        } else {
            std::cout << "Error: index not available" << std::endl
                      << std::endl;
        }
    }
}

void show_commands() {
    std::cout << "Available Commands:" << std::endl;
    std::cout << std::endl;
    std::cout << "[h] Show commands" << std::endl;
    std::cout << "[s] Search a word" << std::endl;
    std::cout << "[o] Open a new file" << std::endl;
    std::cout << "[x] Exit" << std::endl;
    std::cout << std::endl;
    std::cout << "Enter an option: ";
}

// Muestra métricas de construcción del trie: nodos, memoria y profundidad.
template <typename T>
void mostrarEstadisticasTrie(const T& trie, const std::string& tipo) {
    std::cout << "File was read correctly." << std::endl;
    std::cout << SEPARADOR << std::endl;
    std::cout << "Total nodes created in the " << tipo
              << " Trie: " << trie.getNodos() << std::endl;
    const std::size_t memoriaMB = trie.calcularMemoria() / (1024 * 1024);
    std::cout << "Memory used by the " << tipo << " Trie: " << memoriaMB
              << " MB" << std::endl;
    std::cout << "Average height of the " << tipo
              << " Trie: " << trie.calcularProfundidadMedia() << std::endl;
    std::cout << SEPARADOR << std::endl;
}

// Muestra el total de palabras con el prefijo y las 5 primeras o las 5 más
// frecuentes, según la preferencia elegida al arrancar.
template <typename T>
void imprimirAutocompletado(const T& trie, const std::string& w,
                            int& nodosVisitados, int op) {
    // Primero se recuentan todas las palabras del prefijo.
    int nodosAux1 = 0;
    std::vector<std::string> todasPalabras =
        trie.contarPalabrasPrefijo(w, nodosAux1);

    if (todasPalabras.empty()) return;

    const int totalPalabras = static_cast<int>(todasPalabras.size());

    int nodosAux2 = 0;
    std::vector<std::string> top5;
    if (op == 1) {
        top5 = trie.contarPalabrasPrefijo(w, nodosAux2);
        if (top5.size() > 5) top5.erase(top5.begin() + 5, top5.end());
    } else {
        top5 = trie.obtenerTop5MasFrecuentes(w, nodosAux2);
    }

    std::cout << "Total words found with the prefix '" << w
              << "': " << totalPalabras << std::endl;
    std::cout << "Top " << top5.size() << " most frequent words:" << std::endl
              << std::endl;

    int count = 0;
    for (const std::string& it : top5) {
        if (!it.empty()) {
            std::cout << it << "     ";
            ++count;
            if (count % 5 == 0) std::cout << std::endl;
        }
    }
    if (count % 5 != 0) std::cout << std::endl;

    nodosVisitados += nodosAux1 + nodosAux2;
    std::cout << SEPARADOR << std::endl;
}

// Busca una palabra o frase (varias palabras separadas por espacios) y mide
// el tiempo y los nodos visitados.
template <typename T>
void searchCommand(const T& trie, int op) {
    std::cout << "Enter the word you want to search: ";
    std::string words;
    std::cin.ignore();
    if (!std::getline(std::cin, words)) {
        return;
    }

    std::stringstream ss(words);
    std::string word;
    std::vector<std::string> frase;
    while (ss >> word) {
        word = limpiarPalabra(word);
        if (!word.empty()) {
            frase.push_back(word);
        }
    }

    const auto inicio = std::chrono::high_resolution_clock::now();
    int nodosVisitados = 0;
    const std::vector<std::pair<int, int>> lineaPos =
        trie.buscarFrase(frase, nodosVisitados);
    const auto fin = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double, std::milli> duracion = fin - inicio;

    if (!lineaPos.empty()) {
        std::cout << "Word found in lines: ";
        int count = 0;
        for (std::size_t i = 0; i < lineaPos.size(); ++i) {
            std::cout << lineaPos[i].first << "     ";
            ++count;
            if (count % 16 == 0) std::cout << std::endl;
        }
        std::cout << std::endl << std::endl
                  << "Total words founds: " << count << std::endl;
        std::cout << SEPARADOR << std::endl;
        if (frase.size() == 1) {
            imprimirAutocompletado(trie, word, nodosVisitados, op);
        }
    } else {
        std::cout << "Couldn't find the word '" << word << "'" << std::endl;
        std::cout << SEPARADOR << std::endl;
        if (frase.size() == 1) {
            imprimirAutocompletado(trie, word, nodosVisitados, op);
        }
    }

    std::cout << "Search took: " << duracion.count() << " ms" << std::endl;
    std::cout << "Visited nodes: " << nodosVisitados << std::endl;
    std::cout << SEPARADOR << std::endl;
}

// Vacía el trie actual y carga en él un fichero nuevo.
template <typename T>
void openCommand(const std::vector<std::string>& lista, T& trie, bool& valido,
                 const std::string& tipo) {
    std::cout << "Open file:" << std::endl;
    trie.clear();
    valido = false;
    readFile(valido, lista, trie);
    if (!valido) return;
    mostrarEstadisticasTrie(trie, tipo);
}

// Bucle principal de comandos de la aplicación.
template <typename T>
void searchApplication(const std::vector<std::string>& lista, T& trie,
                       bool& valido, int op, const std::string& tipo) {
    char command;
    while (valido && (std::cin >> command) && command != 'x') {
        if (command == 's') {
            searchCommand(trie, op);
            std::cout << "Enter an option: ";
        } else if (command == 'o') {
            openCommand(lista, trie, valido, tipo);
            if (valido) std::cout << "Enter an option: ";
        } else {
            show_commands();
        }
    }
}

template <typename T>
void ejecutarBusqueda(const std::string& tipoNombre,
                      const std::vector<std::string>& lista, int op) {
    T trie;
    bool valido = false;
    readFile(valido, lista, trie);
    if (!valido) return;
    mostrarEstadisticasTrie(trie, tipoNombre);
    show_commands();
    searchApplication(lista, trie, valido, op, tipoNombre);
}

}  // namespace

int main() {
    const std::string type = selectType();
    if (type.empty()) return 0;

    const int op = selectPrefixAlgorithm();
    if (op < 0) return 0;

    const std::vector<std::string> lista = listarArchivosTxt();
    if (type == "normal") {
        ejecutarBusqueda<NodoTrie>("normal", lista, op);
    } else {
        ejecutarBusqueda<NodoRadixTrie>("radix", lista, op);
    }
}
