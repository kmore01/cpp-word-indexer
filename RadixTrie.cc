#include <algorithm>
#include <cctype>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

// Implementación Radix Trie
class RadixTrieNode {
   public:
    string edgeLabel;
    unordered_map<char, unique_ptr<RadixTrieNode>> children;
    bool isEndOfWord;
    int wordCount;
    vector<int> lineNumbers;

    RadixTrieNode(const string& label = "") : edgeLabel(label), isEndOfWord(false), wordCount(0) {}
};

class RadixTrie {
   private:
    unique_ptr<RadixTrieNode> root;
    int totalNodes;
    int totalWords;

   public:
    RadixTrie()
        : root(make_unique<RadixTrieNode>()), totalNodes(1), totalWords(0) {}

    void insert(const string& word, int lineNumber = -1) {
        insertHelper(root.get(), word, 0, lineNumber);
    }

    bool search(const string& word) {
        return searchHelper(root.get(), word, 0);
    }

    vector<int> searchLines(const string& word) {
        return searchLinesHelper(root.get(), word, 0);
    }

    bool startsWith(const string& prefix) {
        return startsWithHelper(root.get(), prefix, 0);
    }

    vector<string> getWordsWithPrefix(const string& prefix) {
        vector<string> result;
        RadixTrieNode* node = findPrefixNode(root.get(), prefix, 0);
        if (node != nullptr) {
            dfsCollectWords(node, prefix, result);
        }
        return result;
    }

    int getNodeCount() const { return totalNodes; }
    int getWordCount() const { return totalWords; }

    int getMaxDepth() { return calculateMaxDepth(root.get(), 0); }

    vector<string> getAllWords() {
        vector<string> result;
        dfsCollectWords(root.get(), "", result);
        sort(result.begin(), result.end());  // Ordenar alfabéticamente
        return result;
    }

   private:
    void insertHelper(RadixTrieNode* node, const string& word, int index,
                      int lineNumber) {
        if (index == word.length()) {
            if (!node->isEndOfWord) {
                node->isEndOfWord = true;
                totalWords++;
            }
            node->wordCount++;
            if (lineNumber != -1) {
                node->lineNumbers.push_back(lineNumber);
            }
            return;
        }

        char c = word[index];

        if (node->children.find(c) == node->children.end()) {
            // Crear nuevo nodo con el resto de la palabra
            string remainingWord = word.substr(index);
            auto newNode = make_unique<RadixTrieNode>(remainingWord);
            newNode->isEndOfWord = true;
            newNode->wordCount = 1;
            if (lineNumber != -1) {
                newNode->lineNumbers.push_back(lineNumber);
            }
            node->children[c] = move(newNode);
            totalNodes++;
            totalWords++;
            return;
        }

        RadixTrieNode* child = node->children[c].get();
        string& edgeLabel = child->edgeLabel;

        // Encontrar el prefijo común
        int commonLength = 0;
        int maxLength = min(edgeLabel.length(), word.length() - index);

        while (commonLength < maxLength and
               edgeLabel[commonLength] == word[index + commonLength]) {
            commonLength++;
        }

        if (commonLength == edgeLabel.length()) {
            // La etiqueta completa coincide, continuar en el hijo
            insertHelper(child, word, index + commonLength, lineNumber);
        } else {
            // Necesitamos dividir el nodo
            splitNode(child, commonLength);
            insertHelper(child, word, index + commonLength, lineNumber);
        }
    }

    void splitNode(RadixTrieNode* node, int splitIndex) {
        string originalLabel = node->edgeLabel;
        string newLabel = originalLabel.substr(splitIndex);

        // Crear nuevo nodo hijo
        auto newChild = make_unique<RadixTrieNode>(newLabel);
        newChild->children = move(node->children);
        newChild->isEndOfWord = node->isEndOfWord;
        newChild->wordCount = node->wordCount;
        newChild->lineNumbers = move(node->lineNumbers);

        // Actualizar nodo actual
        node->edgeLabel = originalLabel.substr(0, splitIndex);
        node->children.clear();
        node->children[newLabel[0]] = move(newChild);
        node->isEndOfWord = false;
        node->wordCount = 0;
        node->lineNumbers.clear();

        totalNodes++;
    }

    bool searchHelper(RadixTrieNode* node, const string& word, int index) {
        if (index == word.length()) {
            return node->isEndOfWord;
        }

        char c = word[index];
        if (node->children.find(c) == node->children.end()) {
            return false;
        }

        RadixTrieNode* child = node->children[c].get();
        string& edgeLabel = child->edgeLabel;

        if (word.length() - index < edgeLabel.length()) {
            return false;
        }

        for (int i = 0; i < edgeLabel.length(); i++) {
            if (word[index + i] != edgeLabel[i]) {
                return false;
            }
        }

        return searchHelper(child, word, index + edgeLabel.length());
    }

    vector<int> searchLinesHelper(RadixTrieNode* node, const string& word,
                                  int index) {
        if (index == word.length()) {
            if (node->isEndOfWord) {
                return node->lineNumbers;
            }
            return {};
        }

        char c = word[index];
        if (node->children.find(c) == node->children.end()) {
            return {};
        }

        RadixTrieNode* child = node->children[c].get();
        string& edgeLabel = child->edgeLabel;

        if (word.length() - index < edgeLabel.length()) {
            return {};
        }

        for (int i = 0; i < edgeLabel.length(); i++) {
            if (word[index + i] != edgeLabel[i]) {
                return {};
            }
        }

        return searchLinesHelper(child, word, index + edgeLabel.length());
    }

    bool startsWithHelper(RadixTrieNode* node, const string& prefix,
                          int index) {
        if (index == prefix.length()) {
            return true;
        }

        char c = prefix[index];
        if (node->children.find(c) == node->children.end()) {
            return false;
        }

        RadixTrieNode* child = node->children[c].get();
        string& edgeLabel = child->edgeLabel;

        int remainingPrefix = prefix.length() - index;
        int commonLength = min(remainingPrefix, (int)edgeLabel.length());

        for (int i = 0; i < commonLength; i++) {
            if (prefix[index + i] != edgeLabel[i]) {
                return false;
            }
        }

        if (remainingPrefix <= edgeLabel.length()) {
            return true;
        }

        return startsWithHelper(child, prefix, index + edgeLabel.length());
    }

    RadixTrieNode* findPrefixNode(RadixTrieNode* node, const string& prefix,
                                  int index) {
        if (index == prefix.length()) {
            return node;
        }

        char c = prefix[index];
        if (node->children.find(c) == node->children.end()) {
            return nullptr;
        }

        RadixTrieNode* child = node->children[c].get();
        string& edgeLabel = child->edgeLabel;

        int remainingPrefix = prefix.length() - index;
        int commonLength = min(remainingPrefix, (int)edgeLabel.length());

        for (int i = 0; i < commonLength; i++) {
            if (prefix[index + i] != edgeLabel[i]) {
                return nullptr;
            }
        }

        if (remainingPrefix <= edgeLabel.length()) {
            return child;
        }

        return findPrefixNode(child, prefix, index + edgeLabel.length());
    }

    void dfsCollectWords(RadixTrieNode* node, const string& currentWord,
                         vector<string>& result) {
        if (node->isEndOfWord) {
            result.push_back(currentWord);
        }

        for (auto& child : node->children) {
            dfsCollectWords(child.second.get(),
                            currentWord + child.second->edgeLabel, result);
        }
    }

    int calculateMaxDepth(RadixTrieNode* node, int depth) {
        if (node->children.empty()) {
            return depth;
        }

        int maxDepth = depth;
        for (auto& child : node->children) {
            maxDepth = max(
                maxDepth,
                calculateMaxDepth(child.second.get(),
                                  depth + child.second->edgeLabel.length()));
        }

        return maxDepth;
    }
};

class TextProcessor {
   public:
    static void processFileWords(const string& filename, RadixTrie& trie) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: No se pudo abrir el archivo " << filename << endl;
            return;
        }

        string line;
        int lineNumber = 1;

        while (getline(file, line)) {
            istringstream iss(line);
            string word;

            while (iss >> word) {
                string cleanedWord = cleanWord(word);
                if (!cleanedWord.empty()) {
                    trie.insert(cleanedWord, lineNumber);
                }
            }
            lineNumber++;
        }

        file.close();
    }

   private:
    static string cleanWord(const string& word) {
        string cleaned;
        for (char c : word) {
            if (isalnum(c)) {
                cleaned += tolower(c);
            }
        }
        return cleaned;
    }
};

void showUsage(const char* programName) {
    cout << "=== PROCESADOR DE ARCHIVOS CON TRIES ===" << endl;
    cout << "USO: " << programName << " <archivo.txt> [opciones]" << endl;
}

void interactiveSearch(RadixTrie& trie) {
    cout << "\n=== BÚSQUEDA INTERACTIVA ===" << endl;
    cout << "Comandos disponibles:" << endl;
    cout << "  'help' - mostrar ayuda" << endl;
    cout << "  'stats' - mostrar estadísticas" << endl;
    cout << "  'quit' - salir" << endl;
    cout << endl;

    string query;
    while (true) {
        cout << "Buscar> ";
        getline(cin, query);

        if (query == "quit" or query == "exit" or query == "q") {
            cout << "¡Hasta luego!" << endl;
            return;
        }

        if (query == "help") {
            cout << "\nAYUDA:" << endl;
            cout << "- Escribe cualquier palabra para buscarla exactamente" << endl;
            cout << "- Si la palabra existe, verás las líneas donde aparece" << endl;
            cout << "- El programa también mostrará autocompletado" << endl;
            continue;
        }

        if (query == "stats") {
            cout << "\nESTADÍSTICAS:" << endl;
            cout << "Nodos: " << trie.getNodeCount() << endl;
            cout << "Palabras: " << trie.getWordCount() << endl;
            cout << "Profundidad máxima: " << trie.getMaxDepth() << endl;
            continue;
        }

        // Limpiar la consulta (convertir a minúsculas)
        string cleanQuery = query;
        transform(cleanQuery.begin(), cleanQuery.end(), cleanQuery.begin(), ::tolower);

        // Búsqueda exacta
        auto searchStart = chrono::high_resolution_clock::now();
        bool found = trie.search(cleanQuery);
        auto searchEnd = chrono::high_resolution_clock::now();
        auto searchTime = chrono::duration_cast<chrono::nanoseconds>(searchEnd - searchStart);

        cout << "Palabra '" << cleanQuery << "': ";

        if (found) {
            cout << "ENCONTRADA (" << searchTime.count() << " ns)" << endl;
            auto lines = trie.searchLines(cleanQuery);
            if (!lines.empty()) {
                cout << "    Aparece en líneas: ";
                for (size_t i = 0; i < min((size_t)15, lines.size()); i++) {
                    cout << lines[i] << " ";
                }
                if (lines.size() > 15) {
                    cout << "... (y " << (lines.size() - 15) << " más)";
                }
                cout << " [Total: " << lines.size() << " apariciones]" << endl;
            }
        } else {
            cout << "NO ENCONTRADA (" << searchTime.count() << " ns)"
                 << endl;
        }

        // Autocompletado
        auto suggestions = trie.getWordsWithPrefix(cleanQuery);

        // Si la palabra fue encontrada, removerla de las sugerencias para evitar redundancia
        bool hadExactMatch = false;
        if (found and !suggestions.empty()) {
            auto it = find(suggestions.begin(), suggestions.end(), cleanQuery);
            if (it != suggestions.end()) {
                suggestions.erase(it);
                hadExactMatch = true;
            }
        }

        if (!suggestions.empty()) {
            cout << "    Autocompletado (" << suggestions.size()
                 << " sugerencias): ";
            for (size_t i = 0; i < min(size_t(10), suggestions.size()); i++) {
                cout << suggestions[i];
                if (i < min(size_t(9), suggestions.size() - 1)) cout << ", ";
            }
            if (suggestions.size() > 10) cout << ", ...";
            cout << endl;
        } else if (!found) {
            cout << "    No hay sugerencias para este prefijo" << endl;
        } else if (hadExactMatch) {
            cout << "    Esta es la única palabra con este prefijo" << endl;
        } else {
            cout << "    No hay sugerencias para este prefijo" << endl;
        }
        cout << endl;
    }
}

void processFileWithRadixTrie(const string& filename) {
    cout << "\n=== PROCESANDO CON RADIX TRIE ===" << endl;

    auto start = chrono::high_resolution_clock::now();
    RadixTrie trie;
    TextProcessor::processFileWords(filename, trie);
    auto end = chrono::high_resolution_clock::now();
    auto buildTime = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "Tiempo de construcción: " << buildTime.count() << " ms" << endl;
    cout << "Nodos totales: " << trie.getNodeCount() << endl;
    cout << "Palabras únicas: " << trie.getWordCount() << endl;
    cout << "Profundidad máxima: " << trie.getMaxDepth() << endl;

    if (trie.getWordCount() > 0) {
        double avgNodesPerWord =
            (double)trie.getNodeCount() / trie.getWordCount();
        cout << "Promedio nodos/palabra: " << fixed << setprecision(2)
             << avgNodesPerWord << endl;
    }

    interactiveSearch(trie);
}

int main(int argc, char* argv[]) {
    // Si no hay argumentos, mostrar ayuda
    if (argc == 1) {
        showUsage(argv[0]);
        return 0;
    }

    // Procesar argumentos
    string filename;
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-h" or arg == "--help") {
            showUsage(argv[0]);
        } else if (arg[0] != '-') {
            filename = arg;
        } else {
            cout << "Opción desconocida: " << arg << endl;
            showUsage(argv[0]);
        }
    }

    // Verificar que el archivo existe
    ifstream testFile(filename);
    if (!testFile.is_open()) {
        cout << "Error: No se pudo abrir el archivo '" << filename << "'" << endl;
        cout << "   Verifica que:" << endl;
        cout << "   - El archivo existe" << endl;
        cout << "   - Tienes permisos de lectura" << endl;
        cout << "   - La ruta es correcta" << endl;
        return 1;
    }
    testFile.close();

    cout << "\nArchivo: " << filename << endl;

    processFileWithRadixTrie(filename);
    
}