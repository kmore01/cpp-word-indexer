cmake_minimum_required(VERSION 3.16)
project(word_indexer LANGUAGES CXX)

include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

add_library(trie
    src/NodoTrie.cpp
    src/NodoRadixTrie.cpp
)
target_include_directories(trie PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(word_indexer src/main.cpp)
target_link_libraries(word_indexer PRIVATE trie)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(trie PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(word_indexer PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(BUILD_TESTING)
    add_executable(trie_tests tests/test_tries.cpp)
    target_link_libraries(trie_tests PRIVATE trie)
    add_test(NAME trie_tests COMMAND trie_tests)
    if(UNIX)
        add_test(
            NAME cli_eof
            COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_cli_eof.sh
                    $<TARGET_FILE:word_indexer>
                    ${CMAKE_CURRENT_SOURCE_DIR}/data/alice.txt
        )
        set_tests_properties(cli_eof PROPERTIES TIMEOUT 30)
    endif()
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(trie_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()
