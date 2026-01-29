# ====================================
# CONFIGURATION MINIMALE DU PROJET
# ====================================
cmake_minimum_required(VERSION 3.16)
project(Img2txt_2_0 VERSION 2.0 LANGUAGES CXX)

# Standard C++17 obligatoire
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ====================================
# OPTIONS DE COMPILATION
# ====================================
if(MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra)
endif()

# ====================================
# CONFIGURATION Qt (AUTOMOC/AUTOUIC/AUTORCC)
# ====================================
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# IMPORTANT : Dire à CMake où chercher les fichiers .ui
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR})

# Recherche de Qt6 uniquement
find_package(Qt6 REQUIRED COMPONENTS Widgets)
message(STATUS "✓ Qt6 trouvé : ${Qt6_VERSION}")
qt_standard_project_setup()

# ====================================
# FICHIERS SOURCES DU PROJET
# ====================================
set(PROJECT_SOURCES
    main.cpp
    Img2txt/img2txt_20.cpp
    Img2txt/FenetreCalibrage.cpp
    Img2txt/FenetreChargement.cpp
    Img2txt/fonctions.cpp
)

set(PROJECT_HEADERS
    Img2txt/img2txt_20.h
    Img2txt/FenetreCalibrage.h
    Img2txt/FenetreChargement.h
    Img2txt/fonctions.h
)

# Fichiers UI et QRC à la RACINE
set(PROJECT_UI
    ${CMAKE_CURRENT_SOURCE_DIR}/Img2txt_20.ui
)

set(PROJECT_RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Img2txt_20.qrc
)

# ====================================
# CRÉATION DE L'EXÉCUTABLE
# ====================================
qt6_add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    ${PROJECT_UI}
    ${PROJECT_RESOURCES}
)

# ====================================
# LIAISON AVEC Qt
# ====================================
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)

# ====================================
# RÉPERTOIRES D'INCLUSION
# ====================================
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}  # ← Permet de trouver stb_image.h à la racine
    ${CMAKE_CURRENT_SOURCE_DIR}/Img2txt
)

# ====================================
# CONFIGURATION WINDOWS (Pas de console en Release)
# ====================================
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE $<CONFIG:Release>
    )
endif()

# ====================================
# MESSAGES DE DIAGNOSTIC
# ====================================
message(STATUS "========================================")
message(STATUS "  Img2txt 2.0 - Configuration")
message(STATUS "========================================")
message(STATUS "Qt version: ${Qt6_VERSION}")
message(STATUS "Compilateur: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Fichier UI: ${PROJECT_UI}")
message(STATUS "========================================")

# ====================================
# DÉPLOIEMENT AUTOMATIQUE (Windows)
# ====================================
if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    # Trouver windeployqt
    find_program(WINDEPLOYQT_EXECUTABLE 
        NAMES windeployqt 
        PATHS ${Qt6_DIR}/../../../bin
    )
    
    if(WINDEPLOYQT_EXECUTABLE)
        # Déployer après la compilation
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${WINDEPLOYQT_EXECUTABLE} 
                --no-translations 
                --no-system-d3d-compiler 
                --no-opengl-sw
                $<TARGET_FILE:${PROJECT_NAME}>
            COMMENT "Déploiement des dépendances Qt..."
        )
    endif()
endif()

# ====================================
# INSTALLATION (pour Linux AppImage)
# ====================================
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Installer les fichiers de ressources si nécessaire
install(FILES ${PROJECT_UI} ${PROJECT_RESOURCES}
    DESTINATION share/${PROJECT_NAME}
)

# ====================================
# NETTOYAGE AUTOMATIQUE POST-BUILD
# ====================================
if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        # Supprimer les fichiers de débogage
        COMMAND ${CMAKE_COMMAND} -E remove_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/.qt"
        COMMAND ${CMAKE_COMMAND} -E remove "$<TARGET_FILE_DIR:${PROJECT_NAME}>/*.pdb"
        COMMAND ${CMAKE_COMMAND} -E remove "$<TARGET_FILE_DIR:${PROJECT_NAME}>/*.ilk"
        
        COMMENT "Nettoyage des fichiers inutiles..."
        VERBATIM
    )
endif()

# ====================================
# OPTIONS D'OPTIMISATION POUR MSVC EN RELEASE
# ====================================
if(MSVC AND CMAKE_BUILD_TYPE STREQUAL "Release")
    # Optimisations agressives
    target_compile_options(${PROJECT_NAME} PRIVATE 
        /O2        # Optimisation max
        /GL        # Link-time optimization
        /Gy        # Function-level linking
    )
    
    target_link_options(${PROJECT_NAME} PRIVATE
        /LTCG      # Link-time code generation
        /OPT:REF   # Éliminer code mort
        /OPT:ICF   # Fusionner fonctions identiques
    )
endif()