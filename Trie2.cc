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

// ========================================
// PARTE 1: CONSTRUCCIÓN DE TRIES
// ========================================

// Implementación 1: Trie Clásico (Representación estándar)
class TrieNode {
   public:
    unordered_map<char, unique_ptr<TrieNode>> children;
    bool isEndOfWord;
    int wordCount;            // Para contar frecuencias
    vector<int> lineNumbers;  // Líneas donde aparece la palabra

    TrieNode() : isEndOfWord(false), wordCount(0) {}
};

// ========================================
// Implementación 2: Radix Trie (Trie Comprimido) - Versión Optimizada
// ========================================

class RadixTrieNode {
   public:
    string edgeLabel;  // Etiqueta de la arista (puede contener múltiples
                       // caracteres)
    unordered_map<char, unique_ptr<RadixTrieNode>> children;
    bool isEndOfWord;
    int wordCount;
    vector<int> lineNumbers;

    RadixTrieNode(const string& label = "")
        : edgeLabel(label), isEndOfWord(false), wordCount(0) {}
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

// ========================================
// UTILIDADES Y FUNCIONES DE PROCESAMIENTO
// ========================================

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

// ========================================
// FUNCIONES DE DEMOSTRACIÓN Y TESTING
// ========================================

void demonstrateTrieOperations() {
    cout << "=== DEMOSTRACIÓN DE OPERACIONES DE TRIE ===" << endl;
    // Insertar algunas palabras de ejemplo
    vector<string> words = {"plant", "planet", "plate",
                            "plan",  "plane",  "plants"};

    // Radix Trie
    RadixTrie radixTrie;
    cout << "\n--- Radix Trie (Comprimido) ---" << endl;

    for (int i = 0; i < words.size(); i++) {
        radixTrie.insert(words[i], i + 1);
    }

    cout << "Nodos totales: " << radixTrie.getNodeCount() << endl;
    cout << "Palabras totales: " << radixTrie.getWordCount() << endl;
    cout << "Profundidad máxima: " << radixTrie.getMaxDepth() << endl;
}

// ========================================
// FUNCIONES DE MODO INTERACTIVO
// ========================================

void showUsage(const char* programName) {
    cout << "=== PROCESADOR DE ARCHIVOS CON TRIES ===" << endl;
    cout << "USO: " << programName << " <archivo.txt> [opciones]" << endl;
    cout << "\nOpciones:" << endl;
    cout << "  -s, --standard    Usar solo Standard Trie" << endl;
    cout << "  -r, --radix       Usar solo Radix Trie" << endl;
    cout << "  -c, --compare     Comparar ambos tipos (por defecto)" << endl;
    cout << "  -d, --demo        Ejecutar demostración sin archivo" << endl;
    cout << "  -h, --help        Mostrar esta ayuda" << endl;
    cout << "\nEjemplos:" << endl;
    cout << "  " << programName << " archivo.txt" << endl;
    cout << "  " << programName << " archivo.txt --standard" << endl;
    cout << "  " << programName << " archivo.txt --radix" << endl;
    cout << "  " << programName << " archivo.txt --compare" << endl;
    cout << "  " << programName << " --demo" << endl;
}

template <typename TrieType>
void interactiveSearch(TrieType& trie, const string& trieTypeName) {
    cout << "\n=== BÚSQUEDA INTERACTIVA (" << trieTypeName << ") ===" << endl;
    cout << "Comandos disponibles:" << endl;
    cout << "  'help' - mostrar ayuda" << endl;
    cout << "  'stats' - mostrar estadísticas" << endl;
    cout << "  'quit' - salir" << endl;
    cout << endl;

    string query;
    while (true) {
        cout << "🔍 Buscar> ";
        getline(cin, query);

        if (query == "quit" or query == "exit" or query == "q") {
            cout << "¡Hasta luego! 👋" << endl;
            break;
        }

        if (query.empty()) {
            continue;
        }

        if (query == "help") {
            cout << "\n📖 AYUDA:" << endl;
            cout << "- Escribe cualquier palabra para buscarla exactamente"
                 << endl;
            cout << "- Si la palabra existe, verás las líneas donde aparece"
                 << endl;
            cout << "- El programa también mostrará autocompletado" << endl;
            cout << "- Ejemplo: 'alice' buscará exactamente 'alice'" << endl;
            cout << "- Ejemplo: 'al' mostrará todas las palabras que "
                    "empiecen por 'al'"
                 << endl;
            continue;
        }

        if (query == "stats") {
            cout << "\n📊 ESTADÍSTICAS (" << trieTypeName << "):" << endl;
            cout << "Nodos: " << trie.getNodeCount() << endl;
            cout << "Palabras: " << trie.getWordCount() << endl;
            cout << "Profundidad máxima: " << trie.getMaxDepth() << endl;
            continue;
        }

        // Limpiar la consulta (convertir a minúsculas)
        string cleanQuery = query;
        transform(cleanQuery.begin(), cleanQuery.end(), cleanQuery.begin(),
                  ::tolower);

        // Búsqueda exacta
        auto searchStart = chrono::high_resolution_clock::now();
        bool found = trie.search(cleanQuery);
        auto searchEnd = chrono::high_resolution_clock::now();
        auto searchTime =
            chrono::duration_cast<chrono::nanoseconds>(searchEnd - searchStart);

        cout << "Palabra '" << cleanQuery << "': ";

        if (found) {
            cout << "✅ ENCONTRADA (" << searchTime.count() << " ns)" << endl;
            auto lines = trie.searchLines(cleanQuery);
            if (!lines.empty()) {
                cout << "   📍 Aparece en líneas: ";
                for (size_t i = 0; i < min((size_t)15, lines.size()); i++) {
                    cout << lines[i] << " ";
                }
                if (lines.size() > 15) {
                    cout << "... (y " << (lines.size() - 15) << " más)";
                }
                cout << " [Total: " << lines.size() << " apariciones]" << endl;
            }
        } else {
            cout << "❌ NO ENCONTRADA (" << searchTime.count() << " ns)"
                 << endl;
        }

        // Autocompletado
        auto suggestions = trie.getWordsWithPrefix(cleanQuery);

        // Si la palabra fue encontrada, removerla de las sugerencias para
        // evitar redundancia
        bool hadExactMatch = false;
        if (found and !suggestions.empty()) {
            auto it = find(suggestions.begin(), suggestions.end(), cleanQuery);
            if (it != suggestions.end()) {
                suggestions.erase(it);
                hadExactMatch = true;
            }
        }

        if (!suggestions.empty()) {
            cout << "   💡 Autocompletado (" << suggestions.size()
                 << " sugerencias): ";
            for (size_t i = 0; i < min(size_t(10), suggestions.size()); i++) {
                cout << suggestions[i];
                if (i < min(size_t(9), suggestions.size() - 1)) cout << ", ";
            }
            if (suggestions.size() > 10) cout << ", ...";
            cout << endl;
        } else if (!found) {
            cout << "   💡 No hay sugerencias para este prefijo" << endl;
        } else if (hadExactMatch) {
            cout << "   💡 Esta es la única palabra con este prefijo" << endl;
        } else {
            cout << "   💡 No hay sugerencias para este prefijo" << endl;
        }
        cout << endl;
    }
}

void processFileWith(const string& filename) {
    cout << "\n=== PROCESANDO CON STANDARD TRIE ===" << endl;

    auto start = chrono::high_resolution_clock::now();
    trie;
    TextProcessor::processFileWords(filename, trie);
    auto end = chrono::high_resolution_clock::now();
    auto buildTime = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "⏱️  Tiempo de construcción: " << buildTime.count() << " ms" << endl;
    cout << "🔢 Nodos totales: " << trie.getNodeCount() << endl;
    cout << "📝 Palabras únicas: " << trie.getWordCount() << endl;
    cout << "📏 Profundidad máxima: " << trie.getMaxDepth() << endl;

    if (trie.getWordCount() > 0) {
        double avgNodesPerWord =
            (double)trie.getNodeCount() / trie.getWordCount();
        cout << "📊 Promedio nodos/palabra: " << fixed << setprecision(2)
             << avgNodesPerWord << endl;
    }

    interactiveSearch(trie, "Standard Trie");
}

void processFileWithRadixTrie(const string& filename) {
    cout << "\n=== PROCESANDO CON RADIX TRIE ===" << endl;

    auto start = chrono::high_resolution_clock::now();
    RadixTrie trie;
    TextProcessor::processFileWords(filename, trie);
    auto end = chrono::high_resolution_clock::now();
    auto buildTime = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "⏱️  Tiempo de construcción: " << buildTime.count() << " ms" << endl;
    cout << "🔢 Nodos totales: " << trie.getNodeCount() << endl;
    cout << "📝 Palabras únicas: " << trie.getWordCount() << endl;
    cout << "📏 Profundidad máxima: " << trie.getMaxDepth() << endl;

    if (trie.getWordCount() > 0) {
        double avgNodesPerWord =
            (double)trie.getNodeCount() / trie.getWordCount();
        cout << "📊 Promedio nodos/palabra: " << fixed << setprecision(2)
             << avgNodesPerWord << endl;
    }

    interactiveSearch(trie, "Radix Trie");
}

void compareTrieTypes(const string& filename) {
    cout << "\n=== COMPARACIÓN DE TIPOS DE TRIE ===" << endl;
    cout << "Archivo: " << filename << endl;

    // Construir Standard Trie
    cout << "\n--- Standard Trie ---" << endl;
    auto start = chrono::high_resolution_clock::now();
    ;
    TextProcessor::processFileWords(filename, );
    auto end = chrono::high_resolution_clock::now();
    auto standardTime =
        chrono::duration_cast<chrono::milliseconds>(end - start);

    // Construir Radix Trie
    cout << "--- Radix Trie ---" << endl;
    start = chrono::high_resolution_clock::now();
    RadixTrie radixTrie;
    TextProcessor::processFileWords(filename, radixTrie);
    end = chrono::high_resolution_clock::now();
    auto radixTime = chrono::duration_cast<chrono::milliseconds>(end - start);

    // Mostrar comparación
    cout << "\n=== RESULTADOS DE LA COMPARACIÓN ===" << endl;
    cout << left << setw(20) << "Métrica" << setw(15) << "Standard Trie"
         << setw(15) << "Radix Trie" << "Mejora" << endl;
    cout << string(65, '-') << endl;

    cout << left << setw(20) << "Tiempo (ms):" << setw(15)
         << standardTime.count() << setw(15) << radixTime.count();
    if (standardTime.count() > 0 and radixTime.count() > 0) {
        double speedup = (double)standardTime.count() / radixTime.count();
        cout << fixed << setprecision(2) << speedup << "x más rápido";
    }
    cout << endl;

    cout << left << setw(20) << "Nodos:" << setw(15) <<.getNodeCount()
         << setw(15) << radixTrie.getNodeCount();
    double nodeReduction =
        ((double)(.getNodeCount() - radixTrie.getNodeCount()) /
             .getNodeCount()) *
        100;
    cout << fixed << setprecision(1) << nodeReduction << "% menos nodos";
    cout << endl;

    cout << left << setw(20) << "Palabras:" << setw(15) <<.getWordCount()
         << setw(15) << radixTrie.getWordCount() << "Igual" << endl;

    cout << left << setw(20) << "Profundidad:" << setw(15) <<.getMaxDepth()
         << setw(15) << radixTrie.getMaxDepth();
    if (.getMaxDepth() == radixTrie.getMaxDepth()) {
        cout << "Igual";
    } else if (radixTrie.getMaxDepth() <.getMaxDepth()) {
        cout << "Menor profundidad";
    } else {
        cout << "Mayor profundidad";
    }
    cout << endl;

    // Benchmark de búsquedas - usar palabras del archivo real
    cout << "\n=== BENCHMARK DE BÚSQUEDAS ===" << endl;

    // Obtener una muestra de palabras del trie para hacer el benchmark más
    // realista
    auto allWords =.getAllWords();
    vector<string> testQueries;

    if (allWords.size() >= 10) {
        // Tomar una muestra representativa del archivo
        // 1. Palabras más comunes (primeras en orden alfabético que suelen ser
        // artículos/conectores)
        size_t maxFirst = min(size_t(3), allWords.size());
        for (size_t i = 0; i < maxFirst; i++) {
            testQueries.push_back(allWords[i]);
        }

        // 2. Palabras del medio del alfabeto
        size_t middle = allWords.size() / 2;
        for (size_t i = 0; i < 3 and (middle + i) < allWords.size(); i++) {
            testQueries.push_back(allWords[middle + i]);
        }

        // 3. Palabras del final
        size_t startIdx = allWords.size() >= 3 ? allWords.size() - 3 : 0;
        for (size_t i = startIdx;
             i < allWords.size() and testQueries.size() < 7; i++) {
            testQueries.push_back(allWords[i]);
        }

        // 4. Siempre agregar una palabra que no existe
        testQueries.push_back("__noexiste__");
    } else {
        // Si hay pocas palabras, usar todas + una inexistente
        testQueries = allWords;
        testQueries.push_back("__noexiste__");
    }

    cout << "Probando con " << testQueries.size()
         << " palabras del archivo:" << endl;

    cout << left << setw(18) << "Palabra" << setw(18) << "Standard (ns)"
         << setw(18) << "Radix (ns)" << "Resultado" << endl;
    cout << string(75, '-') << endl;

    for (const auto& query : testQueries) {
        // Test Standard Trie
        auto searchStart = chrono::high_resolution_clock::now();
        bool foundStandard =.search(query);
        auto searchEnd = chrono::high_resolution_clock::now();
        auto standardSearchTime =
            chrono::duration_cast<chrono::nanoseconds>(searchEnd - searchStart);

        // Test Radix Trie
        searchStart = chrono::high_resolution_clock::now();
        bool foundRadix = radixTrie.search(query);
        searchEnd = chrono::high_resolution_clock::now();
        auto radixSearchTime =
            chrono::duration_cast<chrono::nanoseconds>(searchEnd - searchStart);

        // Truncar palabra si es muy larga para mantener alineación
        string displayWord = query;
        if (displayWord.length() > 17) {
            displayWord = displayWord.substr(0, 14) + "...";
        }

        cout << left << setw(18) << displayWord << setw(18)
             << standardSearchTime.count() << setw(18)
             << radixSearchTime.count();

        if (foundStandard and foundRadix) {
            cout << "✅ Encontrada";
        } else if (!foundStandard and !foundRadix) {
            cout << "❌ No encontrada";
        } else {
            cout << "⚠️  Inconsistencia";
        }
        cout << endl;
    }

    // Elegir qué trie usar para modo interactivo
    cout << "\n¿Qué implementación quieres usar para búsqueda interactiva?"
         << endl;
    cout << "1. Standard Trie" << endl;
    cout << "2. Radix Trie" << endl;
    cout << "3. Salir" << endl;
    cout << "Opción (1-3): ";

    int choice;
    cin >> choice;
    cin.ignore();  // Limpiar el buffer

    switch (choice) {
        case 1:
            interactiveSearch(, "Standard Trie");
            break;
        case 2:
            interactiveSearch(radixTrie, "Radix Trie");
            break;
        case 3:
            cout << "¡Hasta luego!" << endl;
            break;
        default:
            cout << "Opción inválida. Saliendo..." << endl;
            break;
    }
}

// ========================================
// FUNCIÓN PRINCIPAL
// ========================================

int main(int argc, char* argv[]) {
    cout << "PROYECTO: ÁRBOLES LEXICOGRÁFICOS EFICIENTES" << endl;
    cout << "PARTE 1: CONSTRUCCIÓN DE TRIES" << endl;
    cout << "=============================================" << endl;

    // Si no hay argumentos, mostrar ayuda
    if (argc == 1) {
        showUsage(argv[0]);
        return 0;
    }

    // Procesar argumentos
    string filename;
    string mode = "compare";  // Por defecto comparar ambos

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];

        if (arg == "-h" or arg == "--help") {
            showUsage(argv[0]);
            return 0;
        } else if (arg == "-d" or arg == "--demo") {
            // Ejecutar demostración original
            demonstrateTrieOperations();

            // Crear un conjunto de palabras de prueba para benchmark
            vector<string> testWords;
            vector<string> baseWords = {
                "algorithm", "data",        "structure",  "computer",
                "science",   "programming", "software",   "hardware",
                "network",   "database",    "artificial", "intelligence"};

            for (const auto& word : baseWords) {
                testWords.push_back(word);
                testWords.push_back(word + "s");
                testWords.push_back(word + "ing");
            }

            benchmarkTries(testWords);
            return 0;
        } else if (arg == "-s" or arg == "--standard") {
            mode = "standard";
        } else if (arg == "-r" or arg == "--radix") {
            mode = "radix";
        } else if (arg == "-c" or arg == "--compare") {
            mode = "compare";
        } else if (arg[0] != '-') {
            filename = arg;
        } else {
            cout << "Opción desconocida: " << arg << endl;
            showUsage(argv[0]);
            return 1;
        }
    }

    if (filename.empty()) {
        cout << "Error: Debes especificar un archivo o usar --demo" << endl;
        showUsage(argv[0]);
        return 1;
    }

    // Verificar que el archivo existe
    ifstream testFile(filename);
    if (!testFile.is_open()) {
        cerr << "❌ Error: No se pudo abrir el archivo '" << filename << "'"
             << endl;
        cerr << "   Verifica que:" << endl;
        cerr << "   - El archivo existe" << endl;
        cerr << "   - Tienes permisos de lectura" << endl;
        cerr << "   - La ruta es correcta" << endl;
        return 1;
    }
    testFile.close();

    cout << "\n📁 Archivo: " << filename << endl;
    cout << "🔧 Modo: " << mode << endl;

    // Ejecutar según el modo seleccionado
    if (mode == "standard") {
        processFileWith(filename);
    } else if (mode == "radix") {
        processFileWithRadixTrie(filename);
    } else if (mode == "compare") {
        compareTrieTypes(filename);
    }

    return 0;
}