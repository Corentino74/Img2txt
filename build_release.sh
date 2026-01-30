#!/bin/bash
# Script de build et packaging pour Linux

set -e  # Arrêter en cas d'erreur

# Configuration, donnees de version
VERSION="3.0"
APP_NAME="Img2txt"
RELEASE_DIR="release"
ARCHIVE_NAME="${APP_NAME}_v${VERSION}_Linux_x64"

echo -e "Img2txt -> Release v${VERSION}${NC}"

#Nettoyage
rm -rf build release #supprimer les anciens dossiers

# Config CMake
cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS_RELEASE="-O3 -DNDEBUG -s" \
    -DCMAKE_EXE_LINKER_FLAGS="-s"
    
# COMPILATION
echo -e "Par pitié Compile..."
cmake --build build --config Release --parallel $(nproc)

# Packaging
# création de l'archive finale
mkdir -p "${RELEASE_DIR}/${ARCHIVE_NAME}"

# Copier l'exécutable et le renommer
cp "build/Img2txt_2_0" "${RELEASE_DIR}/${ARCHIVE_NAME}/${APP_NAME}"
chmod +x "${RELEASE_DIR}/${ARCHIVE_NAME}/${APP_NAME}"

# Strip pour réduire la taille (enlève les symboles de débogage)
strip --strip-all "${RELEASE_DIR}/${ARCHIVE_NAME}/${APP_NAME}"

# Optionnel : Utiliser UPX si disponible pour compresser l'exécutable
if command -v upx &> /dev/null; then
    echo -e "   → Compression UPX..."
    upx --best --lzma "${RELEASE_DIR}/${ARCHIVE_NAME}/${APP_NAME}" 2>/dev/null || true
fi

# Créer un script de lancement (optionnel, pour gérer LD_LIBRARY_PATH si besoin)
cat > "${RELEASE_DIR}/${ARCHIVE_NAME}/run.sh" << 'EOF'
#!/bin/bash
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export LD_LIBRARY_PATH="$DIR:$LD_LIBRARY_PATH"
"$DIR/Img2txt" "$@"
EOF
chmod +x "${RELEASE_DIR}/${ARCHIVE_NAME}/run.sh"

# Compression de l'archive finale
cd "${RELEASE_DIR}"
tar -czf "${ARCHIVE_NAME}.tar.gz" "${ARCHIVE_NAME}"
cd ..


EXE_PATH="${RELEASE_DIR}/${ARCHIVE_NAME}/${APP_NAME}"
EXE_SIZE=$(du -h "$EXE_PATH" | cut -f1)
TOTAL_SIZE=$(du -sh "${RELEASE_DIR}/${ARCHIVE_NAME}" | cut -f1)
ZIP_SIZE=$(du -h "${RELEASE_DIR}/${ARCHIVE_NAME}.tar.gz" | cut -f1)

echo -e "  Exécutable      : ${EXE_SIZE}"
echo -e "  Archive finale  : ${ZIP_SIZE}"
echo -e "\n  📦 Archive : ${RELEASE_DIR}/${ARCHIVE_NAME}.tar.gz${NC}"

#Et enfin : Nettoyer le dossier build
rm -rf build