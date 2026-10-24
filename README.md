# Indexador de palabras con Trie y Radix Trie

Indexador de texto en C++17 que construye un árbol lexicográfico a partir de un
fichero `.txt` y permite búsquedas tipo *Ctrl+F* (palabras y frases) y
autocompletado por prefijo. El proyecto implementa y compara dos estructuras:
un **trie estándar** y un **radix trie** (trie comprimido).

Trabajo original de la asignatura de **Algoritmia**.

**Autores:** Enzo Miquel, Guillem Revuelta, Kevin Moreno y Jiahao Wang.

---

## Motivación

Buscar una palabra en un texto grande recorriéndolo de principio a fin cuesta
`O(n)` por consulta. Un árbol lexicográfico indexa el texto una sola vez para
que cada búsqueda dependa de la longitud de la palabra (`O(m)`) y no del tamaño
del fichero. Además, la estructura compartida de prefijos permite responder de
forma natural consultas de autocompletado.

El proyecto mantiene dos implementaciones para estudiar el compromiso entre
tiempo y espacio:

- el **trie estándar** es más simple y rápido de recorrer, pero consume más
  memoria;
- el **radix trie** comprime cadenas de nodos con un solo hijo, reduce el número
  de nodos y la memoria, a costa de comparar cadenas completas en cada arista.

## Estructuras y algoritmo

### Trie estándar (`NodoTrie`)

Cada nodo representa un carácter y guarda un `unordered_map<char, NodoTrie*>`
con sus hijos. Una palabra es un camino desde la raíz hasta un nodo marcado como
hoja. En cada hoja se almacenan las posiciones `(línea, posición)` en las que
aparece la palabra.

### Radix trie (`NodoRadixTrie`)

Cada arista puede etiquetarse con una cadena en lugar de un carácter, usando un
`unordered_map<string, NodoRadixTrie*>`. La inserción compara el prefijo común
entre la palabra y las aristas existentes y, cuando es necesario, crea un nodo
intermedio para dividir una arista. Así se eliminan las cadenas de nodos con un
único hijo.

### Búsqueda y autocompletado

- **Búsqueda de palabra:** se desciende por los caracteres (o aristas) y se
  devuelven las posiciones almacenadas en la hoja.
- **Búsqueda de frase:** se obtienen las ocurrencias de cada palabra y se
  comprueba que aparezcan en posiciones consecutivas dentro de la misma línea.
- **Autocompletado:** se localiza el nodo del prefijo y se recorre su subárbol.
  El programa ofrece dos modos:
  - *first found*: las primeras 5 palabras encontradas;
  - *most frequent*: las 5 palabras con más apariciones. En el radix trie se
    resuelve con **quickselect** (`partition` /
    `encontrarIesimoMayor`); en el trie estándar, con un top-5 acotado durante
    el recorrido.
- **Métricas:** nodos creados, memoria estimada y profundidad media del árbol,
  además de tiempo y nodos visitados por búsqueda.

## Complejidades

| Operación | Trie estándar | Radix trie |
|---|---|---|
| Inserción | `O(m)` | `O(m)` comparaciones de caracteres |
| Búsqueda de palabra | `O(m)` | `O(m)` comparaciones de caracteres |
| Autocompletado (prefijo de longitud `p`) | `O(p + k)` nodos, `k` = nodos del subárbol | `O(p + k)` nodos |
| Top-5 más frecuentes | `O(k)` con top-5 acotado | `O(k)` media, `O(k²)` peor caso (quickselect) |
| Memoria | `O(n·m)` nodos | `O(n)` nodos (comprimido) |

`m` es la longitud de la palabra, `n` el número de palabras y `k` el número de
nodos del subárbol del prefijo.

## Estructura del repositorio

```text
.
├── CMakeLists.txt              # Build principal (CMake + CTest)
├── Makefile                    # Alternativa ligera sin CMake
├── LICENSE                     # MIT
├── README.md
├── .clang-format               # Estilo de código reproducible
├── .gitignore
├── .github/
│   └── workflows/ci.yml        # Compilación y pruebas en GitHub Actions
├── include/
│   └── trie/
│       ├── NodoTrie.hpp        # Interfaz del trie estándar
│       └── NodoRadixTrie.hpp   # Interfaz del radix trie
├── src/
│   ├── NodoTrie.cpp            # Implementación del trie estándar
│   ├── NodoRadixTrie.cpp       # Implementación del radix trie
│   └── main.cpp                # Interfaz de consola y orquestación
├── tests/
│   ├── test_tries.cpp          # Pruebas de humo sobre la API pública
│   └── test_cli_eof.sh         # Regresión de EOF e índices inválidos
├── data/
│   └── alice.txt               # Corpus de ejemplo
└── docs/
    ├── InformeEquip27.pdf              # Informe del proyecto
    ├── informe.tex                     # Fuente LaTeX del informe
    └── resultados_experimentales.txt   # Salidas de las pruebas
```

## Requisitos

- Compilador con C++17 y `std::filesystem`: GCC 9+ o Clang 10+.
- `make` (para la compilación rápida) **o** CMake 3.16+.
- Sistema tipo Unix (Linux/macOS). En Windows se recomienda WSL o MSYS2.

Si no tienes CMake instalado, instálalo según tu sistema:

```bash
# Fedora / RHEL
sudo dnf install cmake

# Debian / Ubuntu
sudo apt install cmake

# macOS (Homebrew)
brew install cmake
```

## Compilación

### Make (recomendado si no tienes CMake)

```bash
make -j
```

El ejecutable se genera en `build/word_indexer`. Para limpiar:

```bash
make clean
```

### CMake

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

El ejecutable se genera en `build/word_indexer`.

> No mezcles ambos sistemas de compilación sobre el mismo directorio `build/`:
> ejecuta `make clean` (o `rm -rf build`) antes de cambiar de uno a otro.

## Ejecución

El programa lista los ficheros `.txt` del **directorio actual**, por lo que la
forma más cómoda es ejecutarlo desde `data/`, donde está el corpus de ejemplo:

```bash
cd data
../build/word_indexer
```

Si prefieres ejecutarlo desde la raíz, el menú no encontrará ficheros y deberás
usar la opción `[0] Enter name manually` indicando la ruta, por ejemplo
`data/alice.txt`.

### Ejemplo de sesión

```text
Select the type of Trie to use:

[1] Normal Trie
[2] Radix Trie

Enter an option: 2
Select the autocomplete preference:

[1] First found
[2] Most frequent

Enter an option: 2
==================================
Select a file from the list below:

[1] alice.txt
[0] Enter name manually

Enter an option: 1
==================================
The file selected was 'alice.txt'
Reading file 'alice.txt'...
File was read correctly.
==================================
Total nodes created in the radix Trie: 4034
Memory used by the radix Trie: 1 MB
Average height of the radix Trie: 5.32362
==================================
Available Commands:

[h] Show commands
[s] Search a word
[o] Open a new file
[x] Exit

Enter an option: s
Enter the word you want to search: alice
Word found in lines: 58     146     190     ...
```

Comandos disponibles:

| Comando | Acción |
|---|---|
| `s` | Buscar una palabra o una frase (palabras separadas por espacios) |
| `o` | Cargar otro fichero |
| `h` | Mostrar la ayuda |
| `x` | Salir |

## Formato de los ficheros de entrada

- Texto plano, una o más palabras por línea.
- Cada palabra se normaliza: se descartan los caracteres que no sean letras y
  se convierte a minúsculas.
- La posición de cada palabra se guarda como `(número de línea, índice dentro
  de la línea)`. Las frases solo se consideran válidas si sus palabras son
  consecutivas dentro de la misma línea.

Puedes añadir cualquier `.txt` al directorio desde el que ejecutes el programa;
no es necesario modificar el código.

## Pruebas

Con CMake/CTest:

```bash
ctest --test-dir build --output-on-failure
```

O con el Makefile:

```bash
make test
```

`make test` ejecuta además `tests/test_cli_eof.sh`, que comprueba que el
programa termina correctamente con la entrada cerrada (EOF), rechaza índices
inválidos y completa una búsqueda real con ambos tries. La misma batería se
ejecuta en CI (`.github/workflows/ci.yml`) con CMake y CTest.

Las pruebas insertan palabras y frases en ambos tries y verifican búsquedas,
posiciones, recuento por prefijo, top-5 y reutilización tras `clear()` usando
únicamente la API pública. No modifican la lógica de las estructuras.

## Resultados experimentales

Con `alice.txt` (mismo proceso, compilación `Release`):

| Métrica | Trie estándar | Radix trie |
|---|---|---|
| Nodos creados | 9695 | 4034 |
| Memoria estimada | 2 MB | 1 MB |
| Profundidad media | 7.57 | 5.32 |

Las trazas completas de búsquedas y autocompletado están en
[`docs/resultados_experimentales.txt`](docs/resultados_experimentales.txt) y el
análisis detallado en [`docs/InformeEquip27.pdf`](docs/InformeEquip27.pdf).

## Limitaciones conocidas

- `getNodos()` devuelve el contador acumulado de nodos creados en el proceso,
  no el número de nodos vivos: al recargar un fichero el contador sigue
  creciendo, aunque `clear()` libera la memoria anterior.
- Los nodos no son copiables (`NodoTrie` y `NodoRadixTrie` tienen la copia
  deshabilitada); se usan por referencia.
- Al cargar un fichero, las palabras se guardan por líneas; una frase solo
  coincide si sus palabras son consecutivas en la misma línea.
- La entrada se procesa de forma interactiva: si el flujo de entrada se cierra
  (EOF), el programa termina sin bloquearse.
- `calcularMemoria()` es una **estimación** basada en `sizeof` y en el número de
  buckets; no mide el asignador real.
- El corpus incluido es solo una muestra. Para reproducir los experimentos con
  corpus mayores, añade los `.txt` correspondientes (no se versionan).

## Licencia

Distribuido bajo la licencia [MIT](LICENSE).
