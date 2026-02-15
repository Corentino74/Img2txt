# CmakeLists.txt - Configuration de la compilation multi-plateformes pour Img2txt
cmake_minimum_required(VERSION 3.16)

# Versionnage et métadonnées de l'application
set(APP_VERSION_MAJOR 4)
set(APP_VERSION_MINOR 0)
set(APP_VERSION "${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}")
set(APP_NAME "Img2txt")
set(APP_AUTHOR "Corentin Chitwood")
set(APP_COPYRIGHT "Copyright (C) 2026 ${APP_AUTHOR}")
set(APP_DESCRIPTION "Convertisseur d'images en ASCII Art")

project(${APP_NAME} VERSION ${APP_VERSION} LANGUAGES CXX)

# Standard C++17 obligatoire
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC) #compilation sous Visual Studio
    add_compile_options(/W4 /utf-8)
else() 
    add_compile_options(-Wall -Wextra)
endif()

# CONFIGURATION Qt (AUTOMOC/AUTOUIC/AUTORCC)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}) # Permet de trouver les fichiers .ui à la racine

# Recherche des composants Qt6 nécessaires 
find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

# FICHIERS SOURCES DU PROJET
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

set(PROJECT_UI
    ${CMAKE_CURRENT_SOURCE_DIR}/Img2txt_20.ui
)

set(PROJECT_RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Img2txt_20.qrc
)

# CRÉATION DE L'EXÉCUTABLE // oh boy, my favorite :D 
set(EXECUTABLE_NAME "${APP_NAME}_V${APP_VERSION}")

# Création de l'exécutable à partir des sources, headers, UI et ressources
qt6_add_executable(${EXECUTABLE_NAME}
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    ${PROJECT_UI}
    ${PROJECT_RESOURCES}
)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE Qt6::Widgets)
target_include_directories(${EXECUTABLE_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}  # Permet de trouver stb_image.h à la racine
    ${CMAKE_CURRENT_SOURCE_DIR}/Img2txt
)

# Configuration pour windows // je vous jure passez sur Linux
if(WIN32)
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        WIN32_EXECUTABLE $<CONFIG:Release>
    )
    
    # Ajouter le fichier de ressources Windows (.rc) pour l'icône et les métadonnées
    target_sources(${EXECUTABLE_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/app_resources.rc
    )
endif()

# DÉPLOIEMENT AUTOMATIQUE (Windows)
if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    # Trouver windeployqt
    find_program(WINDEPLOYQT_EXECUTABLE 
        NAMES windeployqt 
        PATHS ${Qt6_DIR}/../../../bin
    )
    
    if(WINDEPLOYQT_EXECUTABLE)
        # Déployer après la compilation
        add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
            COMMAND ${WINDEPLOYQT_EXECUTABLE} 
                --no-translations 
                --no-system-d3d-compiler 
                --no-opengl-sw
                $<TARGET_FILE:${EXECUTABLE_NAME}>
            COMMENT "Déploiement des dépendances Qt..."
        )
    endif()
endif()

# INSTALLATION (Linux AppImage)
install(TARGETS ${EXECUTABLE_NAME}
    RUNTIME DESTINATION bin
)

# Installer les fichiers de ressources si nécessaire
install(FILES ${PROJECT_UI} ${PROJECT_RESOURCES}
    DESTINATION share/${EXECUTABLE_NAME}
)

# NETTOYAGE AUTOMATIQUE POST-BUILD ET OPTI (si Release sur Windows)
if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")

    # Optimisations agressives
    target_compile_options(${EXECUTABLE_NAME} PRIVATE 
        /O2        # Optimisation max
        /GL        # Link-time optimization
        /Gy        # Function-level linking
    )
    
    target_link_options(${EXECUTABLE_NAME} PRIVATE
        /LTCG      # Link-time code generation
        /OPT:REF   # Éliminer code mort
        /OPT:ICF   # Fusionner fonctions identiques
    )

    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        # Supprimer les fichiers de débogage
        COMMAND ${CMAKE_COMMAND} -E remove_directory "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/.qt"
        COMMAND ${CMAKE_COMMAND} -E remove "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/*.pdb"
        COMMAND ${CMAKE_COMMAND} -E remove "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/*.ilk"
        VERBATIM
    )
endif()

# OPTIMISATIONS DE COMPILATION (Release sur Windows)
# if(MSVC AND CMAKE_BUILD_TYPE STREQUAL "Release")
#    # Optimisations agressives
#    target_compile_options(${EXECUTABLE_NAME} PRIVATE 
#         /O2        # Optimisation max
#         /GL        # Link-time optimization
#         /Gy        # Function-level linking
#     )
    
#     target_link_options(${EXECUTABLE_NAME} PRIVATE
#         /LTCG      # Link-time code generation
#         /OPT:REF   # Éliminer code mort
#         /OPT:ICF   # Fusionner fonctions identiques
#     )
# endif()