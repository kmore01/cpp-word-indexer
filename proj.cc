#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <cctype>

// ========================================
// PARTE 1: CONSTRUCCIÓN DE TRIES
// ========================================

// Implementación 1: Trie Clásico (Representación estándar)
class TrieNode {
public:
    std::unordered_map<char, std::unique_ptr<TrieNode>> children;
    bool isEndOfWord;
    int wordCount;  // Para contar frecuencias
    std::vector<int> lineNumbers; // Líneas donde aparece la palabra
    
    TrieNode() : isEndOfWord(false), wordCount(0) {}
};

class StandardTrie {
private:
    std::unique_ptr<TrieNode> root;
    int totalNodes;
    int totalWords;
    
public:
    StandardTrie() : root(std::make_unique<TrieNode>()), totalNodes(1), totalWords(0) {}
    
    // Insertar una palabra con su número de línea
    void insert(const std::string& word, int lineNumber = -1) {
        TrieNode* current = root.get();
        
        for (char c : word) {
            if (current->children.find(c) == current->children.end()) {
                current->children[c] = std::make_unique<TrieNode>();
                totalNodes++;
            }
            current = current->children[c].get();
        }
        
        if (!current->isEndOfWord) {
            current->isEndOfWord = true;
            totalWords++;
        }
        current->wordCount++;
        
        if (lineNumber != -1) {
            current->lineNumbers.push_back(lineNumber);
        }
    }
    
    // Buscar una palabra
    bool search(const std::string& word) {
        TrieNode* current = root.get();
        
        for (char c : word) {
            if (current->children.find(c) == current->children.end()) {
                return false;
            }
            current = current->children[c].get();
        }
        
        return current->isEndOfWord;
    }
    
    // Buscar líneas donde aparece una palabra
    std::vector<int> searchLines(const std::string& word) {
        TrieNode* current = root.get();
        
        for (char c : word) {
            if (current->children.find(c) == current->children.end()) {
                return {};
            }
            current = current->children[c].get();
        }
        
        if (current->isEndOfWord) {
            return current->lineNumbers;
        }
        return {};
    }
    
    // Verificar si existe un prefijo
    bool startsWith(const std::string& prefix) {
        TrieNode* current = root.get();
        
        for (char c : prefix) {
            if (current->children.find(c) == current->children.end()) {
                return false;
            }
            current = current->children[c].get();
        }
        
        return true;
    }
    
    // Obtener todas las palabras con un prefijo dado
    std::vector<std::string> getWordsWithPrefix(const std::string& prefix) {
        std::vector<std::string> result;
        TrieNode* current = root.get();
        
        // Navegar hasta el nodo del prefijo
        for (char c : prefix) {
            if (current->children.find(c) == current->children.end()) {
                return result; // Prefijo no encontrado
            }   
            current = current->children[c].get();
        }
        
        // DFS para encontrar todas las palabras
        dfsCollectWords(current, prefix, result);
        return result;
    }
    
    // Estadísticas del trie
    int getNodeCount() const { return totalNodes; }
    int getWordCount() const { return totalWords; }
    
    int getMaxDepth() {
        return calculateMaxDepth(root.get(), 0);
    }
    
    std::vector<std::string> getAllWords() {
        std::vector<std::string> result;
        dfsCollectWords(root.get(), "", result);
        std::sort(result.begin(), result.end()); // Ordenar alfabéticamente
        return result;
    }
    
private:
    void dfsCollectWords(TrieNode* node, const std::string& currentWord, std::vector<std::string>& result) {
        if (node->isEndOfWord) {
            result.push_back(currentWord);
        }
        
        for (auto& child : node->children) {
            dfsCollectWords(child.second.get(), currentWord + child.first, result);
        }
    }
    
    int calculateMaxDepth(TrieNode* node, int depth) {
        if (node->children.empty()) {
            return depth;
        }
        
        int maxDepth = depth;
        for (auto& child : node->children) {
            maxDepth = std::max(maxDepth, calculateMaxDepth(child.second.get(), depth + 1));
        }
        
        return maxDepth;
    }
};

// ========================================
// Implementación 2: Radix Trie (Trie Comprimido) - Versión Optimizada
// ========================================

class RadixTrieNode {
public:
    std::string edgeLabel; // Etiqueta de la arista (puede contener múltiples caracteres)
    std::unordered_map<char, std::unique_ptr<RadixTrieNode>> children;
    bool isEndOfWord;
    int wordCount;
    std::vector<int> lineNumbers;
    
    RadixTrieNode(const std::string& label = "") 
        : edgeLabel(label), isEndOfWord(false), wordCount(0) {}
};

class RadixTrie {
private:
    std::unique_ptr<RadixTrieNode> root;
    int totalNodes;
    int totalWords;
    
public:
    RadixTrie() : root(std::make_unique<RadixTrieNode>()), totalNodes(1), totalWords(0) {}
    
    void insert(const std::string& word, int lineNumber = -1) {
        insertHelper(root.get(), word, 0, lineNumber);
    }
    
    bool search(const std::string& word) {
        return searchHelper(root.get(), word, 0);
    }
    
    std::vector<int> searchLines(const std::string& word) {
        return searchLinesHelper(root.get(), word, 0);
    }
    
    bool startsWith(const std::string& prefix) {
        return startsWithHelper(root.get(), prefix, 0);
    }
    
    std::vector<std::string> getWordsWithPrefix(const std::string& prefix) {
        std::vector<std::string> result;
        RadixTrieNode* node = findPrefixNode(root.get(), prefix, 0);
        if (node != nullptr) {
            dfsCollectWords(node, prefix, result);
        }
        return result;
    }
    
    int getNodeCount() const { return totalNodes; }
    int getWordCount() const { return totalWords; }
    
    int getMaxDepth() {
        return calculateMaxDepth(root.get(), 0);
    }
    
    std::vector<std::string> getAllWords() {
        std::vector<std::string> result;
        dfsCollectWords(root.get(), "", result);
        std::sort(result.begin(), result.end()); // Ordenar alfabéticamente
        return result;
    }
    
private:
    void insertHelper(RadixTrieNode* node, const std::string& word, int index, int lineNumber) {
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
            std::string remainingWord = word.substr(index);
            auto newNode = std::make_unique<RadixTrieNode>(remainingWord);
            newNode->isEndOfWord = true;
            newNode->wordCount = 1;
            if (lineNumber != -1) {
                newNode->lineNumbers.push_back(lineNumber);
            }
            node->children[c] = std::move(newNode);
            totalNodes++;
            totalWords++;
            return;
        }
        
        RadixTrieNode* child = node->children[c].get();
        std::string& edgeLabel = child->edgeLabel;
        
        // Encontrar el prefijo común
        int commonLength = 0;
        int maxLength = std::min(edgeLabel.length(), word.length() - index);
        
        while (commonLength < maxLength && 
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
        std::string originalLabel = node->edgeLabel;
        std::string newLabel = originalLabel.substr(splitIndex);
        
        // Crear nuevo nodo hijo
        auto newChild = std::make_unique<RadixTrieNode>(newLabel);
        newChild->children = std::move(node->children);
        newChild->isEndOfWord = node->isEndOfWord;
        newChild->wordCount = node->wordCount;
        newChild->lineNumbers = std::move(node->lineNumbers);
        
        // Actualizar nodo actual
        node->edgeLabel = originalLabel.substr(0, splitIndex);
        node->children.clear();
        node->children[newLabel[0]] = std::move(newChild);
        node->isEndOfWord = false;
        node->wordCount = 0;
        node->lineNumbers.clear();
        
        totalNodes++;
    }
    
    bool searchHelper(RadixTrieNode* node, const std::string& word, int index) {
        if (index == word.length()) {
            return node->isEndOfWord;
        }
        
        char c = word[index];
        if (node->children.find(c) == node->children.end()) {
            return false;
        }
        
        RadixTrieNode* child = node->children[c].get();
        std::string& edgeLabel = child->edgeLabel;
        
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
    
    std::vector<int> searchLinesHelper(RadixTrieNode* node, const std::string& word, int index) {
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
        std::string& edgeLabel = child->edgeLabel;
        
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
    
    bool startsWithHelper(RadixTrieNode* node, const std::string& prefix, int index) {
        if (index == prefix.length()) {
            return true;
        }
        
        char c = prefix[index];
        if (node->children.find(c) == node->children.end()) {
            return false;
        }
        
        RadixTrieNode* child = node->children[c].get();
        std::string& edgeLabel = child->edgeLabel;
        
        int remainingPrefix = prefix.length() - index;
        int commonLength = std::min(remainingPrefix, (int)edgeLabel.length());
        
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
    
    RadixTrieNode* findPrefixNode(RadixTrieNode* node, const std::string& prefix, int index) {
        if (index == prefix.length()) {
            return node;
        }
        
        char c = prefix[index];
        if (node->children.find(c) == node->children.end()) {
            return nullptr;
        }
        
        RadixTrieNode* child = node->children[c].get();
        std::string& edgeLabel = child->edgeLabel;
        
        int remainingPrefix = prefix.length() - index;
        int commonLength = std::min(remainingPrefix, (int)edgeLabel.length());
        
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
    
    void dfsCollectWords(RadixTrieNode* node, const std::string& currentWord, std::vector<std::string>& result) {
        if (node->isEndOfWord) {
            result.push_back(currentWord);
        }
        
        for (auto& child : node->children) {
            dfsCollectWords(child.second.get(), currentWord + child.second->edgeLabel, result);
        }
    }
    
    int calculateMaxDepth(RadixTrieNode* node, int depth) {
        if (node->children.empty()) {
            return depth;
        }
        
        int maxDepth = depth;
        for (auto& child : node->children) {
            maxDepth = std::max(maxDepth, calculateMaxDepth(child.second.get(), depth + child.second->edgeLabel.length()));
        }
        
        return maxDepth;
    }
};

// ========================================
// UTILIDADES Y FUNCIONES DE PROCESAMIENTO
// ========================================

class TextProcessor {
public:
    // Procesar un archivo de texto y construir el trie con todas las palabras
    static void processFileWords(const std::string& filename, StandardTrie& trie) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: No se pudo abrir el archivo " << filename << std::endl;
            return;
        }
        
        std::string line;
        int lineNumber = 1;
        
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string word;
            
            while (iss >> word) {
                // Limpiar la palabra de signos de puntuación
                std::string cleanedWord = cleanWord(word);
                if (!cleanedWord.empty()) {
                    trie.insert(cleanedWord, lineNumber);
                }
            }
            lineNumber++;
        }
        
        file.close();
    }
    
    static void processFileWords(const std::string& filename, RadixTrie& trie) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: No se pudo abrir el archivo " << filename << std::endl;
            return;
        }
        
        std::string line;
        int lineNumber = 1;
        
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string word;
            
            while (iss >> word) {
                std::string cleanedWord = cleanWord(word);
                if (!cleanedWord.empty()) {
                    trie.insert(cleanedWord, lineNumber);
                }
            }
            lineNumber++;
        }
        
        file.close();
    }
    
    // Procesar archivo creando sufijos (para búsquedas de frases)
    static void processFileSuffixes(const std::string& filename, StandardTrie& trie, int maxSuffixLength = 50) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: No se pudo abrir el archivo " << filename << std::endl;
            return;
        }
        
        std::string line;
        int lineNumber = 1;
        
        while (std::getline(file, line)) {
            // Crear sufijos de cada línea para permitir búsquedas de frases
            for (int i = 0; i < line.length(); i++) {
                if (std::isalnum(line[i]) || line[i] == ' ') {
                    int suffixLength = std::min(maxSuffixLength, (int)(line.length() - i));
                    std::string suffix = line.substr(i, suffixLength);
                    trie.insert(suffix, lineNumber);
                }
            }
            lineNumber++;
        }
        
        file.close();
    }
    
private:
    static std::string cleanWord(const std::string& word) {
        std::string cleaned;
        for (char c : word) {
            if (std::isalnum(c)) {
                cleaned += std::tolower(c);
            }
        }
        return cleaned;
    }
};

// ========================================
// FUNCIONES DE DEMOSTRACIÓN Y TESTING
// ========================================

void demonstrateTrieOperations() {
    std::cout << "=== DEMOSTRACIÓN DE OPERACIONES DE TRIE ===" << std::endl;
    
    // Trie estándar
    StandardTrie standardTrie;
    std::cout << "\n--- Trie Estándar ---" << std::endl;
    
    // Insertar algunas palabras de ejemplo
    std::vector<std::string> words = {"plant", "planet", "plate", "plan", "plane", "plants"};
    for (int i = 0; i < words.size(); i++) {
        standardTrie.insert(words[i], i + 1);
    }
    
    std::cout << "Palabras insertadas: ";
    for (const auto& word : words) {
        std::cout << word << " ";
    }
    std::cout << std::endl;
    
    std::cout << "Nodos totales: " << standardTrie.getNodeCount() << std::endl;
    std::cout << "Palabras totales: " << standardTrie.getWordCount() << std::endl;
    std::cout << "Profundidad máxima: " << standardTrie.getMaxDepth() << std::endl;
    
    // Probar búsquedas
    std::vector<std::string> searchWords = {"plant", "planet", "plants", "tree", "pla"};
    for (const auto& word : searchWords) {
        bool found = standardTrie.search(word);
        std::cout << "Buscar '" << word << "': " << (found ? "ENCONTRADO" : "NO ENCONTRADO") << std::endl;
    }
    
    // Probar prefijos
    std::string prefix = "pla";
    auto wordsWithPrefix = standardTrie.getWordsWithPrefix(prefix);
    std::cout << "Palabras con prefijo '" << prefix << "': ";
    for (const auto& word : wordsWithPrefix) {
        std::cout << word << " ";
    }
    std::cout << std::endl;
    
    // Radix Trie
    RadixTrie radixTrie;
    std::cout << "\n--- Radix Trie (Comprimido) ---" << std::endl;
    
    for (int i = 0; i < words.size(); i++) {
        radixTrie.insert(words[i], i + 1);
    }
    
    std::cout << "Nodos totales: " << radixTrie.getNodeCount() << std::endl;
    std::cout << "Palabras totales: " << radixTrie.getWordCount() << std::endl;
    std::cout << "Profundidad máxima: " << radixTrie.getMaxDepth() << std::endl;
    
    // Comparar eficiencia
    std::cout << "\n--- Comparación de Eficiencia ---" << std::endl;
    std::cout << "Reducción de nodos: " << 
        ((double)(standardTrie.getNodeCount() - radixTrie.getNodeCount()) / standardTrie.getNodeCount() * 100) 
        << "%" << std::endl;
}

void benchmarkTries(const std::vector<std::string>& words) {
    std::cout << "\n=== BENCHMARK DE TRIES ===" << std::endl;
    
    // Benchmark Standard Trie
    auto start = std::chrono::high_resolution_clock::now();
    StandardTrie standardTrie;
    for (const auto& word : words) {
        standardTrie.insert(word);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto standardInsertTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Benchmark Radix Trie
    start = std::chrono::high_resolution_clock::now();
    RadixTrie radixTrie;
    for (const auto& word : words) {
        radixTrie.insert(word);
    }
    end = std::chrono::high_resolution_clock::now();
    auto radixInsertTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Palabras procesadas: " << words.size() << std::endl;
    std::cout << "Standard Trie - Tiempo inserción: " << standardInsertTime.count() << " μs" << std::endl;
    std::cout << "Standard Trie - Nodos: " << standardTrie.getNodeCount() << std::endl;
    std::cout << "Radix Trie - Tiempo inserción: " << radixInsertTime.count() << " μs" << std::endl;
    std::cout << "Radix Trie - Nodos: " << radixTrie.getNodeCount() << std::endl;
    std::cout << "Reducción de nodos: " << 
        ((double)(standardTrie.getNodeCount() - radixTrie.getNodeCount()) / standardTrie.getNodeCount() * 100) 
        << "%" << std::endl;
}

// ========================================
// FUNCIONES DE MODO INTERACTIVO
// ========================================

void showUsage(const char* programName) {
    std::cout << "=== PROCESADOR DE ARCHIVOS CON TRIES ===" << std::endl;
    std::cout << "USO: " << programName << " <archivo.txt> [opciones]" << std::endl;
    std::cout << "\nOpciones:" << std::endl;
    std::cout << "  -s, --standard    Usar solo Standard Trie" << std::endl;
    std::cout << "  -r, --radix       Usar solo Radix Trie" << std::endl;
    std::cout << "  -c, --compare     Comparar ambos tipos (por defecto)" << std::endl;
    std::cout << "  -d, --demo        Ejecutar demostración sin archivo" << std::endl;
    std::cout << "  -h, --help        Mostrar esta ayuda" << std::endl;
    std::cout << "\nEjemplos:" << std::endl;
    std::cout << "  " << programName << " archivo.txt" << std::endl;
    std::cout << "  " << programName << " archivo.txt --standard" << std::endl;
    std::cout << "  " << programName << " archivo.txt --radix" << std::endl;
    std::cout << "  " << programName << " archivo.txt --compare" << std::endl;
    std::cout << "  " << programName << " --demo" << std::endl;
}

template<typename TrieType>
void interactiveSearch(TrieType& trie, const std::string& trieTypeName) {
    std::cout << "\n=== BÚSQUEDA INTERACTIVA (" << trieTypeName << ") ===" << std::endl;
    std::cout << "Comandos disponibles:" << std::endl;
    std::cout << "  'help' - mostrar ayuda" << std::endl;
    std::cout << "  'stats' - mostrar estadísticas" << std::endl;
    std::cout << "  'quit' - salir" << std::endl;
    std::cout << std::endl;
    
    std::string query;
    while (true) {
        std::cout << "🔍 Buscar> ";
        std::getline(std::cin, query);
        
        if (query == "quit" || query == "exit" || query == "q") {
            std::cout << "¡Hasta luego! 👋" << std::endl;
            break;
        }
        
        if (query.empty()) {
            continue;
        }
        
        if (query == "help") {
            std::cout << "\n📖 AYUDA:" << std::endl;
            std::cout << "- Escribe cualquier palabra para buscarla exactamente" << std::endl;
            std::cout << "- Si la palabra existe, verás las líneas donde aparece" << std::endl;
            std::cout << "- El programa también mostrará autocompletado" << std::endl;
            std::cout << "- Ejemplo: 'alice' buscará exactamente 'alice'" << std::endl;
            std::cout << "- Ejemplo: 'al' mostrará todas las palabras que empiecen por 'al'" << std::endl;
            continue;
        }
        
        if (query == "stats") {
            std::cout << "\n📊 ESTADÍSTICAS (" << trieTypeName << "):" << std::endl;
            std::cout << "Nodos: " << trie.getNodeCount() << std::endl;
            std::cout << "Palabras: " << trie.getWordCount() << std::endl;
            std::cout << "Profundidad máxima: " << trie.getMaxDepth() << std::endl;
            continue;
        }
        
        // Limpiar la consulta (convertir a minúsculas)
        std::string cleanQuery = query;
        std::transform(cleanQuery.begin(), cleanQuery.end(), cleanQuery.begin(), ::tolower);
        
        // Búsqueda exacta
        auto searchStart = std::chrono::high_resolution_clock::now();
        bool found = trie.search(cleanQuery);
        auto searchEnd = std::chrono::high_resolution_clock::now();
        auto searchTime = std::chrono::duration_cast<std::chrono::nanoseconds>(searchEnd - searchStart);
        
        std::cout << "Palabra '" << cleanQuery << "': ";
        
        if (found) {
            std::cout << "✅ ENCONTRADA (" << searchTime.count() << " ns)" << std::endl;
            auto lines = trie.searchLines(cleanQuery);
            if (!lines.empty()) {
                std::cout << "   📍 Aparece en líneas: ";
                for (size_t i = 0; i < std::min((size_t)15, lines.size()); i++) {
                    std::cout << lines[i] << " ";
                }
                if (lines.size() > 15) {
                    std::cout << "... (y " << (lines.size() - 15) << " más)";
                }
                std::cout << " [Total: " << lines.size() << " apariciones]" << std::endl;
            }
        } else {
            std::cout << "❌ NO ENCONTRADA (" << searchTime.count() << " ns)" << std::endl;
        }
        
        // Autocompletado
        auto suggestions = trie.getWordsWithPrefix(cleanQuery);
        
        // Si la palabra fue encontrada, removerla de las sugerencias para evitar redundancia
        bool hadExactMatch = false;
        if (found && !suggestions.empty()) {
            auto it = std::find(suggestions.begin(), suggestions.end(), cleanQuery);
            if (it != suggestions.end()) {
                suggestions.erase(it);
                hadExactMatch = true;
            }
        }
        
        if (!suggestions.empty()) {
            std::cout << "   💡 Autocompletado (" << suggestions.size() << " sugerencias): ";
            for (size_t i = 0; i < std::min(size_t(10), suggestions.size()); i++) {
                std::cout << suggestions[i];
                if (i < std::min(size_t(9), suggestions.size() - 1)) std::cout << ", ";
            }
            if (suggestions.size() > 10) std::cout << ", ...";
            std::cout << std::endl;
        } else if (!found) {
            std::cout << "   💡 No hay sugerencias para este prefijo" << std::endl;
        } else if (hadExactMatch) {
            std::cout << "   💡 Esta es la única palabra con este prefijo" << std::endl;
        } else {
            std::cout << "   💡 No hay sugerencias para este prefijo" << std::endl;
        }
        std::cout << std::endl;
    }
}

void processFileWithStandardTrie(const std::string& filename) {
    std::cout << "\n=== PROCESANDO CON STANDARD TRIE ===" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    StandardTrie trie;
    TextProcessor::processFileWords(filename, trie);
    auto end = std::chrono::high_resolution_clock::now();
    auto buildTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "⏱️  Tiempo de construcción: " << buildTime.count() << " ms" << std::endl;
    std::cout << "🔢 Nodos totales: " << trie.getNodeCount() << std::endl;
    std::cout << "📝 Palabras únicas: " << trie.getWordCount() << std::endl;
    std::cout << "📏 Profundidad máxima: " << trie.getMaxDepth() << std::endl;
    
    if (trie.getWordCount() > 0) {
        double avgNodesPerWord = (double)trie.getNodeCount() / trie.getWordCount();
        std::cout << "📊 Promedio nodos/palabra: " << std::fixed << std::setprecision(2) << avgNodesPerWord << std::endl;
    }
    
    interactiveSearch(trie, "Standard Trie");
}

void processFileWithRadixTrie(const std::string& filename) {
    std::cout << "\n=== PROCESANDO CON RADIX TRIE ===" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    RadixTrie trie;
    TextProcessor::processFileWords(filename, trie);
    auto end = std::chrono::high_resolution_clock::now();
    auto buildTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "⏱️  Tiempo de construcción: " << buildTime.count() << " ms" << std::endl;
    std::cout << "🔢 Nodos totales: " << trie.getNodeCount() << std::endl;
    std::cout << "📝 Palabras únicas: " << trie.getWordCount() << std::endl;
    std::cout << "📏 Profundidad máxima: " << trie.getMaxDepth() << std::endl;
    
    if (trie.getWordCount() > 0) {
        double avgNodesPerWord = (double)trie.getNodeCount() / trie.getWordCount();
        std::cout << "📊 Promedio nodos/palabra: " << std::fixed << std::setprecision(2) << avgNodesPerWord << std::endl;
    }
    
    interactiveSearch(trie, "Radix Trie");
}

void compareTrieTypes(const std::string& filename) {
    std::cout << "\n=== COMPARACIÓN DE TIPOS DE TRIE ===" << std::endl;
    std::cout << "Archivo: " << filename << std::endl;
    
    // Construir Standard Trie
    std::cout << "\n--- Standard Trie ---" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    StandardTrie standardTrie;
    TextProcessor::processFileWords(filename, standardTrie);
    auto end = std::chrono::high_resolution_clock::now();
    auto standardTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Construir Radix Trie
    std::cout << "--- Radix Trie ---" << std::endl;
    start = std::chrono::high_resolution_clock::now();
    RadixTrie radixTrie;
    TextProcessor::processFileWords(filename, radixTrie);
    end = std::chrono::high_resolution_clock::now();
    auto radixTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Mostrar comparación
    std::cout << "\n=== RESULTADOS DE LA COMPARACIÓN ===" << std::endl;
    std::cout << std::left << std::setw(20) << "Métrica" << std::setw(15) << "Standard Trie" << std::setw(15) << "Radix Trie" << "Mejora" << std::endl;
    std::cout << std::string(65, '-') << std::endl;
    
    std::cout << std::left << std::setw(20) << "Tiempo (ms):" << std::setw(15) << standardTime.count() << std::setw(15) << radixTime.count();
    if (standardTime.count() > 0 && radixTime.count() > 0) {
        double speedup = (double)standardTime.count() / radixTime.count();
        std::cout << std::fixed << std::setprecision(2) << speedup << "x más rápido";
    }
    std::cout << std::endl;
    
    std::cout << std::left << std::setw(20) << "Nodos:" << std::setw(15) << standardTrie.getNodeCount() << std::setw(15) << radixTrie.getNodeCount();
    double nodeReduction = ((double)(standardTrie.getNodeCount() - radixTrie.getNodeCount()) / standardTrie.getNodeCount()) * 100;
    std::cout << std::fixed << std::setprecision(1) << nodeReduction << "% menos nodos";
    std::cout << std::endl;
    
    std::cout << std::left << std::setw(20) << "Palabras:" << std::setw(15) << standardTrie.getWordCount() << std::setw(15) << radixTrie.getWordCount() << "Igual" << std::endl;
    
    std::cout << std::left << std::setw(20) << "Profundidad:" << std::setw(15) << standardTrie.getMaxDepth() << std::setw(15) << radixTrie.getMaxDepth();
    if (standardTrie.getMaxDepth() == radixTrie.getMaxDepth()) {
        std::cout << "Igual";
    } else if (radixTrie.getMaxDepth() < standardTrie.getMaxDepth()) {
        std::cout << "Menor profundidad";
    } else {
        std::cout << "Mayor profundidad";
    }
    std::cout << std::endl;
    
    // Benchmark de búsquedas - usar palabras del archivo real
    std::cout << "\n=== BENCHMARK DE BÚSQUEDAS ===" << std::endl;
    
    // Obtener una muestra de palabras del trie para hacer el benchmark más realista
    auto allWords = standardTrie.getAllWords();
    std::vector<std::string> testQueries;
    
    if (allWords.size() >= 10) {
        // Tomar una muestra representativa del archivo
        // 1. Palabras más comunes (primeras en orden alfabético que suelen ser artículos/conectores)
        size_t maxFirst = std::min(size_t(3), allWords.size());
        for (size_t i = 0; i < maxFirst; i++) {
            testQueries.push_back(allWords[i]);
        }
        
        // 2. Palabras del medio del alfabeto
        size_t middle = allWords.size() / 2;
        for (size_t i = 0; i < 3 && (middle + i) < allWords.size(); i++) {
            testQueries.push_back(allWords[middle + i]);
        }
        
        // 3. Palabras del final
        size_t startIdx = allWords.size() >= 3 ? allWords.size() - 3 : 0;
        for (size_t i = startIdx; i < allWords.size() && testQueries.size() < 7; i++) {
            testQueries.push_back(allWords[i]);
        }
        
        // 4. Siempre agregar una palabra que no existe
        testQueries.push_back("__noexiste__");
    } else {
        // Si hay pocas palabras, usar todas + una inexistente
        testQueries = allWords;
        testQueries.push_back("__noexiste__");
    }
    
    std::cout << "Probando con " << testQueries.size() << " palabras del archivo:" << std::endl;
    
    std::cout << std::left << std::setw(18) << "Palabra" << std::setw(18) << "Standard (ns)" << std::setw(18) << "Radix (ns)" << "Resultado" << std::endl;
    std::cout << std::string(75, '-') << std::endl;
    
    for (const auto& query : testQueries) {
        // Test Standard Trie
        auto searchStart = std::chrono::high_resolution_clock::now();
        bool foundStandard = standardTrie.search(query);
        auto searchEnd = std::chrono::high_resolution_clock::now();
        auto standardSearchTime = std::chrono::duration_cast<std::chrono::nanoseconds>(searchEnd - searchStart);
        
        // Test Radix Trie
        searchStart = std::chrono::high_resolution_clock::now();
        bool foundRadix = radixTrie.search(query);
        searchEnd = std::chrono::high_resolution_clock::now();
        auto radixSearchTime = std::chrono::duration_cast<std::chrono::nanoseconds>(searchEnd - searchStart);
        
        // Truncar palabra si es muy larga para mantener alineación
        std::string displayWord = query;
        if (displayWord.length() > 17) {
            displayWord = displayWord.substr(0, 14) + "...";
        }
        
        std::cout << std::left << std::setw(18) << displayWord 
                  << std::setw(18) << standardSearchTime.count() 
                  << std::setw(18) << radixSearchTime.count();
        
        if (foundStandard && foundRadix) {
            std::cout << "✅ Encontrada";
        } else if (!foundStandard && !foundRadix) {
            std::cout << "❌ No encontrada";
        } else {
            std::cout << "⚠️  Inconsistencia";
        }
        std::cout << std::endl;
    }
    
    // Elegir qué trie usar para modo interactivo
    std::cout << "\n¿Qué implementación quieres usar para búsqueda interactiva?" << std::endl;
    std::cout << "1. Standard Trie" << std::endl;
    std::cout << "2. Radix Trie" << std::endl;
    std::cout << "3. Salir" << std::endl;
    std::cout << "Opción (1-3): ";
    
    int choice;
    std::cin >> choice;
    std::cin.ignore(); // Limpiar el buffer
    
    switch (choice) {
        case 1:
            interactiveSearch(standardTrie, "Standard Trie");
            break;
        case 2:
            interactiveSearch(radixTrie, "Radix Trie");
            break;
        case 3:
            std::cout << "¡Hasta luego!" << std::endl;
            break;
        default:
            std::cout << "Opción inválida. Saliendo..." << std::endl;
            break;
    }
}

// ========================================
// FUNCIÓN PRINCIPAL
// ========================================

int main(int argc, char* argv[]) {
    std::cout << "PROYECTO: ÁRBOLES LEXICOGRÁFICOS EFICIENTES" << std::endl;
    std::cout << "PARTE 1: CONSTRUCCIÓN DE TRIES" << std::endl;
    std::cout << "=============================================" << std::endl;
    
    // Si no hay argumentos, mostrar ayuda
    if (argc == 1) {
        showUsage(argv[0]);
        return 0;
    }
    
    // Procesar argumentos
    std::string filename;
    std::string mode = "compare"; // Por defecto comparar ambos
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            showUsage(argv[0]);
            return 0;
        } else if (arg == "-d" || arg == "--demo") {
            // Ejecutar demostración original
            demonstrateTrieOperations();
            
            // Crear un conjunto de palabras de prueba para benchmark
            std::vector<std::string> testWords;
            std::vector<std::string> baseWords = {
                "algorithm", "data", "structure", "computer", "science", "programming",
                "software", "hardware", "network", "database", "artificial", "intelligence"
            };
            
            for (const auto& word : baseWords) {
                testWords.push_back(word);
                testWords.push_back(word + "s");
                testWords.push_back(word + "ing");
            }
            
            benchmarkTries(testWords);
            return 0;
        } else if (arg == "-s" || arg == "--standard") {
            mode = "standard";
        } else if (arg == "-r" || arg == "--radix") {
            mode = "radix";
        } else if (arg == "-c" || arg == "--compare") {
            mode = "compare";
        } else if (arg[0] != '-') {
            filename = arg;
        } else {
            std::cout << "Opción desconocida: " << arg << std::endl;
            showUsage(argv[0]);
            return 1;
        }
    }
    
    if (filename.empty()) {
        std::cout << "Error: Debes especificar un archivo o usar --demo" << std::endl;
        showUsage(argv[0]);
        return 1;
    }
    
    // Verificar que el archivo existe
    std::ifstream testFile(filename);
    if (!testFile.is_open()) {
        std::cerr << "❌ Error: No se pudo abrir el archivo '" << filename << "'" << std::endl;
        std::cerr << "   Verifica que:" << std::endl;
        std::cerr << "   - El archivo existe" << std::endl;
        std::cerr << "   - Tienes permisos de lectura" << std::endl;
        std::cerr << "   - La ruta es correcta" << std::endl;
        return 1;
    }
    testFile.close();
    
    std::cout << "\n📁 Archivo: " << filename << std::endl;
    std::cout << "🔧 Modo: " << mode << std::endl;
    
    // Ejecutar según el modo seleccionado
    if (mode == "standard") {
        processFileWithStandardTrie(filename);
    } else if (mode == "radix") {
        processFileWithRadixTrie(filename);
    } else if (mode == "compare") {
        compareTrieTypes(filename);
    }
    
    return 0;
}
