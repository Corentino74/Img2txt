#!/usr/bin/env pwsh
# Script de build et packaging pour Windows

param(
    [string]$Version = "4.0"
)

$ErrorActionPreference = "Stop"

$env:PATH += ";C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin"
$AppName = "Img2txt"
$ExeName = "Img2txt_v$Version"  # Nom de l'exécutable généré par CMake
$QtPath = "C:/Qt/6.10.1/msvc2022_64"

Write-Host "== Img2txt v$Version -> Release Windows =="

# clean du dossier de build si existe
if (Test-Path "build") {
    Remove-Item -Recurse -Force build
}

# Configuration CMake
cmake -B build `
    -DCMAKE_PREFIX_PATH="$QtPath" `
    -DCMAKE_BUILD_TYPE=Release `
    -G "Visual Studio 17 2022" `

if ($LASTEXITCODE -ne 0) { # En cas d'erreur...
    Write-Host "Erreur de configuration CMake"
    exit 1 
}

#  Compilation
cmake --build build --config Release --parallel

if ($LASTEXITCODE -ne 0) { 
    Write-Host "Erreur de compilation"
    exit 1 
}

# Création du packag
$ReleaseDir = "release/${AppName}_v${Version}_Windows_x64" # Créer le dossier de release
New-Item -ItemType Directory -Force -Path $ReleaseDir | Out-Null

# Copier l'exécutable
Copy-Item "build/Release/${ExeName}.exe" "$ReleaseDir/${AppName}.exe"

# Déployer les DLLs Qt (seulement celles nécessaires)
& "$QtPath/bin/windeployqt.exe" `
    --release `
    --no-translations `
    --no-system-d3d-compiler `
    --no-opengl-sw `
    --no-compiler-runtime `
    "$ReleaseDir/${AppName}.exe"

# Supprimer les fichiers inutiles
Remove-Item -Recurse -Force "$ReleaseDir/imageformats" -ErrorAction SilentlyContinue
Remove-Item "$ReleaseDir/*.pdb" -ErrorAction SilentlyContinue

# Compression en ZIP
$ArchiveName = "${AppName}_v${Version}_Windows_x64.zip"
Compress-Archive -Path "$ReleaseDir/*" -DestinationPath "release/$ArchiveName" -Force

# Nettoyer le dossier build
Remove-Item -Recurse -Force build


Write-Host "== Img2txt compilé avec succès =="