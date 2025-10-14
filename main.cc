#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class BuscadorAvanzado {
public:
    // Buscar y resaltar coincidencias
    static void buscarYResaltar(const string& texto, const string& palabra) {
        string textoLower = aMinusculas(texto);
        string palabraLower = aMinusculas(palabra);
        size_t pos = 0;
        int contador = 0;
        
        cout << "Texto original: " << texto << endl;
        cout << "Coincidencias resaltadas: ";
        
        while ((pos = textoLower.find(palabraLower, pos)) != string::npos) {
            // Imprimir texto antes de la coincidencia
            if (pos > 0) {
                cout << texto.substr(0, pos);
            }
            
            // Resaltar coincidencia
            cout << "\033[1;31m"; // Código ANSI para color rojo
            cout << texto.substr(pos, palabra.length());
            cout << "\033[0m"; // Reset color
            
            // Actualizar posición
            pos += palabra.length();
            contador++;
            
            // Si hay más texto después de la coincidencia
            if (pos < texto.length()) {
                textoLower = textoLower.substr(pos);
                texto = texto.substr(pos);
                pos = 0;
            }
        }
        
        if (contador == 0) {
            cout << texto; // Imprimir texto original si no hay coincidencias
        }
        
        cout << endl;
        cout << "Total de coincidencias: " << contador << endl;
    }
    
    // Buscar palabras completas solamente
    static vector<size_t> buscarPalabraCompleta(const string& texto, const string& palabra) {
        vector<size_t> posiciones;
        size_t pos = 0;
        
        while ((pos = texto.find(palabra, pos)) != string::npos) {
            // Verificar si es una palabra completa
            bool inicioOk = (pos == 0 || !esLetraONumero(texto[pos - 1]));
            bool finOk = (pos + palabra.length() == texto.length() || 
                         !esLetraONumero(texto[pos + palabra.length()]));
            
            if (inicioOk && finOk) {
                posiciones.push_back(pos);
            }
            
            pos += palabra.length();
        }
        
        return posiciones;
    }
    
private:
    static string aMinusculas(const string& texto) {
        string resultado = texto;
        for (char& c : resultado) {
            c = tolower(c);
        }
        return resultado;
    }
    
    static bool esLetraONumero(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || 
               (c >= '0' && c <= '9') || c == '_';
    }
};

int main() {
    string texto = "El perro corre. El perro grande corre rápido. perrotono no cuenta.";
    string palabra;
    
    cout << "Texto: " << texto << endl;
    cout << "Ingresa la palabra a buscar: ";
    cin >> palabra;
    
    cout << "\n--- Búsqueda con resaltado ---" << endl;
    BuscadorAvanzado::buscarYResaltar(texto, palabra);
    
    cout << "\n--- Búsqueda de palabra completa ---" << endl;
    auto posiciones = BuscadorAvanzado::buscarPalabraCompleta(texto, palabra);
    
    if (posiciones.empty()) {
        cout << "No se encontró la palabra completa '" << palabra << "'" << endl;
    } else {
        cout << "Palabra completa encontrada en " << posiciones.size() << " posición(es): ";
        for (size_t pos : posiciones) {
            cout << pos << " ";
        }
        cout << endl;
    }
    
    return 0;
}