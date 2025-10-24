# Proyecto Algoritmia

Autores:
- Enzo Miquel
- Guillem Revuelta
- Kevin Moreno
- Jiahao Wang

Descripción
-----------

Este repositorio contiene una implementación de dos tipos de estructuras de trie (normal y radix) y una utilidad de búsqueda/autocompletado sobre ficheros de texto. El programa principal permite seleccionar el tipo de trie, cargar un fichero de texto con palabras, realizar búsquedas de palabras/frases y mostrar estadísticas básicas (nodos creados, memoria, profundidad media).

Estructura relevante
--------------------

- `main.cc` : Programa principal (interfaz de consola)
- `NodoTrie.cc` / `NodoTrie.hh` : Implementación del trie "normal"
- `NodoRadixTrie.cc` / `NodoRadixTrie.hh` : Implementación del radix trie
- `Makefile` : reglas de compilación (usar `make`)

Compilación
-----------

Para compilar el proyecto:

```bash
make
```

Esto generará el ejecutable `main` y los archivos objeto `.o` necesarios.

Ejecución
---------

Una vez compilado, ejecuta:

```bash
./main
```

El programa te pedirá que selecciones el tipo de trie y el fichero a cargar. Sigue las instrucciones por pantalla.

Archivos de prueba
------------------

Se pueden utilizar archivos `.txt` ubicados en el mismo directorio para realizar pruebas.

Limpieza
--------

Para eliminar los archivos objeto y el ejecutable:

```bash
make clean
```