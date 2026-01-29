#!/bin/bash

# Script pour créer une release Linux de Img2Text -> Ce script compile le projet et crée une archive prête pour GitHub Release

set -e  # Arrêter en cas d'erreur

# Créer le dossier de build s'il n'existe pas
if [ ! -d "build" ]; then
    echo -e "Création du dossier build..."
    mkdir build
fi

# Variables de configuration
VERSION="1.0"
APP_NAME="Img2txt"
RELEASE_DIR="release"
ARCHIVE_NAME="${APP_NAME}-v${VERSION}-linux"

# == Compiler le projet ==

# Créer le dossier de build s'il n'existe pas
if [ ! -d "build" ]; then
    echo -e "Création du dossier build"
    mkdir build
fi

# Se déplacer dans le dossier build
cd build

# Configuration avec CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Compilation
cmake --build . --config Release -j$(nproc)

# Vérifier si la compilation a réussi
if [ -f "Img2txt" ]; then
    echo -e "Compilation réussie !"
    echo -e "Exécutable créé: build/Img2txt"
    
    # Afficher la taille de l'exécutable
    SIZE=$(du -h Img2txt | cut -f1)
    echo -e "Taille de l'exécutable: $SIZE"
    
    # Rendre l'exécutable... exécutable
    chmod +x Img2txt
else
    echo -e "Erreur lors de la compilation"
    exit 1
fi


# Créer le dossier de release
if [ -d "$RELEASE_DIR" ]; then
    rm -rf "$RELEASE_DIR"
fi
mkdir -p "$RELEASE_DIR/$ARCHIVE_NAME"

# Copier l'exécutable
cp build/Img2txt "$RELEASE_DIR/$ARCHIVE_NAME/"
strip "$RELEASE_DIR/$ARCHIVE_NAME/Img2txt"  # Réduire la taille


# Créer l'archive
cd "$RELEASE_DIR"
tar -czf "${ARCHIVE_NAME}.tar.gz" "$ARCHIVE_NAME"
cd ..