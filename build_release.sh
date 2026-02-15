#!/bin/bash
# Script de build et packaging pour Linux

set -e

# Configuration
VERSION="4.0"
APP_NAME="Img2txt"
EXE_NAME="${APP_NAME}_V${VERSION}"
RELEASE_DIR="release"

echo "== Img2txt version ${VERSION} Release Build =="

rm -rf build "$RELEASE_DIR" # Nettoyer les anciens builds et releases

# lance CMake pour la configuration en release
cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DCMAKE_CXX_FLAGS_RELEASE="-O3 -DNDEBUG" \
    -DCMAKE_EXE_LINKER_FLAGS="-s"

#-> compilation en release avec optimisations et stripping
cmake --build build --config Release --parallel $(nproc)

# Strip pour reduire la taille, merci le sain Shell
strip --strip-all "build/${EXE_NAME}"

# re-creation du dossier release
ARCHIVE_NAME="${APP_NAME}_v${VERSION}_Linux_x64"
mkdir -p "${RELEASE_DIR}/${ARCHIVE_NAME}"

# Copier executable
cp "build/${EXE_NAME}" "${RELEASE_DIR}/${ARCHIVE_NAME}/${APP_NAME}"
chmod +x "${RELEASE_DIR}/${ARCHIVE_NAME}/${APP_NAME}"

# vider le dossier de build maintenant inutile
rm -rf build

# Et compression pour partage plus facile
cd "${RELEASE_DIR}"
tar -czf "${ARCHIVE_NAME}.tar.gz" "${ARCHIVE_NAME}"

echo -e "== Build fini avec succes ! =="