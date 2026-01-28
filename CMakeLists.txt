cmake_minimum_required(VERSION 3.10)

# Nom et version du projet
project(Img2txt VERSION 1.0 LANGUAGES CXX)

# Standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options de compilation optimisées
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG /GL")
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
endif()

# === ORGANISATION AMÉLIORÉE ===

# En-tête pour stb_image (header-only)
set(STB_HEADERS stb_image.h)

# Sources du projet
set(PROJECT_SOURCES
    Img2txt/Img2txt.cpp
    Img2txt/fonctions.cpp
    Img2txt/stb_impl.cpp  # Implémentation de stb_image
)

set(PROJECT_HEADERS
    Img2txt/fonctions.h
)

# Créer l'exécutable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS})

# Dossiers d'inclusion
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# === INSTALLATION ===
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# === PACKAGING ===
set(CPACK_PACKAGE_NAME "Img2txt")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "Corentin C")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Convertisseur d'image en ASCII art")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

if(WIN32)
    set(CPACK_GENERATOR "ZIP")
else()
    set(CPACK_GENERATOR "TGZ")
endif()

include(CPack)

# === MESSAGES D'INFO ===
message(STATUS "=== Img2txt Configuration ===")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Plateforme: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Type de build: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compilateur: ${CMAKE_CXX_COMPILER_ID}")
