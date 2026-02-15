# Img2txt v4.0

**Convertisseur d'images en ASCII Art** avec support des couleurs et multiples modes de rendu.

![Img2txt v4.0](illustrations/app_icon.png)


## Fonctionnalités

### Modes de rendu

Img2txt propose **3 modes distincts** de conversion d'images en ASCII Art :

#### 1. Mode Simple (Monochrome)
- Conversion en niveaux de gris avec **22 palettes de caractères** prédéfinies
- Palettes disponibles : Normale, Classique, Blocs, Clair Obscur, Nature/Lumière, Médiéval, Détails fins, Aquarelle, Gradients, Cyberpunk, 2-Bit, Haute Définition, etc.
- Option **Inverser les couleurs** pour adapter à différents fonds
- Rendu optimisé avec correction du ratio 2:1 des caractères

#### 2. Mode Couleur 1 cara (Mono-caractère)
- Utilise un **unique caractère** avec variations de couleurs
- Caractères prédéfinis : █ ▓ ▒ ░ @ # * +
- Option de **caractère personnalisé**
- Support de palettes couleur : 1 couleur personnalisée (RGB), 8 couleurs, 16 couleurs ANSI, 32, 64, 128, 256 couleurs xterm

#### 3. Mode Couleur multi (Multi-caractères)
- Combine **variations de caractères ET de couleurs**
- Même gamme de palettes couleur que le mode 1 cara
- Rendu le plus détaillé et expressif
- Option **Inverser les couleurs**

### Fonctionnalités communes

- **Redimensionnement intelligent** avec préservation du ratio d'aspect
- **Aperçu en temps réel** dans l'interface
- **3 formats d'export** :
  - `.txt` : Texte brut (mode Simple uniquement)
  - `.html` : Page web avec couleurs CSS inline et fond personnalisable
  - `.ansi` : Codes d'échappement ANSI pour affichage terminal
- Support des formats d'image : PNG, JPEG, BMP, GIF (via stb_image)
- Interface moderne avec **thème sombre**

---

## Architecture du projet

### Structure des fichiers

```
Img2txt/
├── main.cpp                    # Point d'entrée, fenêtre principale Qt
├── img2txt_20.cpp/h            # Fenêtre principale, navigation entre écrans
├── FenetreChargement.cpp/h     # Écran de chargement d'image
├── FenetreCalibrage.cpp/h      # Écran de calibrage avec 3 modes
├── fonctions.cpp/h             # Fonctions de traitement d'images
├── stb_image.h                 # Bibliothèque de chargement d'images
├── Img2txt_20.ui               # Interface Qt Designer (non utilisée)
├── Img2txt_20.qrc              # Ressources Qt (icônes)
└── illustrations/              # Icônes et images
    └── app_icon.png            # Icône 256x256
```

### Structures de données principales

#### ImagePGM
Structure pour images en **niveaux de gris** (Portable Gray Map) :
```cpp
struct ImagePGM {
    int largeur;
    int hauteur;
    std::vector<uint8_t> pixels;  // 0-255, ligne par ligne
};
```

#### Image
Structure unifiée supportant **couleur ET niveaux de gris** :
```cpp
struct Image {
    int largeur;
    int hauteur;
    bool estCouleur;
    std::vector<uint8_t> pixelsGris;                   // Si !estCouleur
    std::vector<std::tuple<uint8_t,uint8_t,uint8_t>> pixelsRGB;  // Si estCouleur
};
```

#### PaletteCouleur
Représente une palette de caractères avec modes de couleur :
```cpp
struct PaletteCouleur {
    std::string nom;
    ModeRendu mode;               // MONOCHROME, COULEUR_8/16/256, TRUE_COLOR
    std::vector<CouleurRGB> couleurs;
    std::vector<std::string> caracteres;  // UTF-8
};
```

### Architecture Qt

- **img2txt_20** : QMainWindow avec QStackedWidget pour naviguer entre écrans
- **FenetreChargement** : Sélection de fichier, émission du signal `imagePGMChargee`
- **FenetreCalibrage** : QStackedWidget interne avec 3 panels pour les 3 modes
- **Connexions signal/slot** :
  - `FenetreChargement::imagePGMChargee` → `Img2txt_20::onImageChargee`
  - `FenetreCalibrage::retourDemande` → `Img2txt_20::onRetourChargement`

---

## Algorithmes et fonctionnement

### 1. Chargement d'images

#### Images couleur (PNG, JPEG, etc.)
Utilise **stb_image.h** pour décoder les formats :
```cpp
int channels;
unsigned char* data = stbi_load(nomFichier.c_str(), &width, &height, &channels, 3);
// Force 3 canaux (RGB) même pour images avec alpha
```

#### Images PGM (niveaux de gris)
Lecture manuelle du format P5 (PGM binaire) :
- Lecture de l'en-tête : `P5 largeur hauteur maxval`
- Chargement des pixels en bloc
- Normalisation à 255 niveaux si nécessaire

### 2. Conversion en ASCII Art

#### Mode Simple (Monochrome)

**Algorithme de base** :
1. Pour chaque pixel `(x, y)` avec intensité `I` (0-255)
2. Calculer l'index dans la palette : `index = (I × (taille_palette - 1)) / 255`
3. Récupérer le caractère : `palette[index]`
4. **Sauter une ligne sur deux** : `y += 2` (correction ratio 2:1)

**Correction du ratio** : Les caractères ont un ratio hauteur/largeur ≈ 2:1. Pour éviter un étirement vertical, on saute une ligne d'image sur deux.

**Option Inverser** : Inverse l'ordre de la palette pour adapter aux fonds clairs/sombres.

#### Mode Couleur 1 cara (Mono-caractère)

**Algorithme** :
1. Pour chaque pixel `(x, y)` avec couleurs `(R, G, B)`
2. Utiliser **toujours le même caractère**
3. Trouver la couleur la plus proche dans la palette (algorithme ci-dessous)
4. Encoder avec codes couleur HTML/ANSI
5. **Sauter une ligne sur deux** : `y += 2`

#### Mode Couleur multi (Multi-caractères)

**Algorithme** :
1. Pour chaque pixel `(x, y)` avec couleurs `(R, G, B)`
2. Calculer la **luminosité** : `L = 0.299×R + 0.587×G + 0.114×B`
3. Choisir le caractère selon luminosité : `index_char = L × (nb_caractères - 1)`
4. Trouver la **couleur la plus proche** dans la palette
5. Encoder caractère + couleur
6. **Sauter une ligne sur deux** : `y += 2`

### 3. Recherche de couleur proche

Utilise la **distance euclidienne dans l'espace RGB** :

```cpp
int trouverCouleurProche(uint8_t r, uint8_t g, uint8_t b, const PaletteCouleur& palette) {
    int distanceMin = INT_MAX;
    int indexMin = 0;
    
    for (int i = 0; i < palette.couleurs.size(); ++i) {
        int dr = r - palette.couleurs[i].r;
        int dg = g - palette.couleurs[i].g;
        int db = b - palette.couleurs[i].b;
        int distance = dr*dr + dg*dg + db*db;
        
        if (distance < distanceMin) {
            distanceMin = distance;
            indexMin = i;
        }
    }
    
    return indexMin;
}
```

**Complexité** : O(n × m × p) où n = nb_pixels, m = nb_couleurs, p ≈ constante

**Optimisation possible** : Octree ou k-d tree pour réduire à O(n × log m)

### 4. Redimensionnement

Utilise un **moyennage par blocs** (box sampling) :

1. Calculer les ratios : `ratioX = largeur_old / largeur_new`
2. Pour chaque pixel de sortie `(x_out, y_out)` :
   - Calculer la zone source : `[x_start, x_end] × [y_start, y_end]`
   - Moyenner tous les pixels dans cette zone
   - Assigner la moyenne au pixel de sortie

**Avantages** :
- Évite l'aliasing (crénelage)
- Préserve les détails lors de la réduction
- Rapide (pas d'interpolation complexe)

### 5. Export des formats

#### Format .txt (Texte brut)
Écriture directe de la chaîne ASCII générée, sans métadonnées.

#### Format .html (Page web)
Structure complète avec :
- En-tête HTML5 avec charset UTF-8
- CSS inline pour style monospace : `font-family: 'Courier New', 'Consolas', monospace`
- Chaque caractère enrobé dans `<span style="color:rgb(R,G,B)">char</span>`
- Background personnalisable (noir/blanc)
- Line-height et letter-spacing optimisés pour rendu compact

#### Format .ansi (Terminal)
Utilise les **codes d'échappement ANSI** :
- True Color : `\033[38;2;R;G;Bm` pour définir la couleur du texte
- Reset : `\033[0m` en fin de ligne
- Compatible avec terminaux modernes (support 24-bit)

**Visualisation** :
```bash
cat fichier.ansi          # Afficher dans le terminal
less -R fichier.ansi      # Avec pagination
```

---

## Installation

### Dépendances

#### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install build-essential cmake
sudo apt install qt6-base-dev qt6-base-dev-tools
sudo apt install libgl1-mesa-dev
```

#### Linux (Fedora/RHEL)
```bash
sudo dnf install gcc-c++ cmake
sudo dnf install qt6-qtbase-devel
sudo dnf install mesa-libGL-devel
```

#### Windows
1. Installer **Visual Studio 2022** avec "Desktop development with C++"
2. Installer **Qt 6.10.1** (ou supérieur) via l'installateur officiel
3. Installer **CMake** 3.28 ou supérieur

#### macOS
```bash
brew install cmake qt@6
```

---

## Compilation

### Linux

#### Compilation manuelle
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

L'exécutable sera dans `build/Img2txt_v4.0`

#### Build release (script automatique)
```bash
chmod +x build_release.sh
./build_release.sh
```

Crée une archive `release/Img2txt_v4.0_Linux_x64.tar.gz` avec :
- Exécutable optimisé et strippé (580 Ko)
- README.txt avec instructions d'installation

**Extraction et exécution** :
```bash
tar -xzf Img2txt_v4.0_Linux_x64.tar.gz
cd Img2txt_v4.0_Linux_x64
./Img2txt
```

### Windows

#### Avec script PowerShell
```powershell
.\build_release.ps1
```

Crée un dossier `release/Img2txt_v4.0_Windows_x64/` avec :
- `Img2txt_v4.0.exe` avec icône
- DLLs Qt nécessaires (via windeployqt)
- Plugins Qt (platforms, styles, etc.)

#### Compilation manuelle
```powershell
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

---

## Utilisation

### 1. Lancer l'application

**Linux** :
```bash
./Img2txt_v4.0
```

**Windows** :
Double-cliquer sur `Img2txt_v4.0.exe`

### 2. Charger une image

- Cliquer sur **"Charger une image"**
- Sélectionner un fichier (PNG, JPEG, BMP, GIF, PGM)
- L'image est automatiquement chargée en niveaux de gris

### 3. Choisir le mode

#### Mode Simple
1. Sélectionner une **palette de caractères** (22 disponibles)
2. Cocher **"Inverser les couleurs"** si nécessaire
3. Ajuster les dimensions avec les spinbox
4. Cliquer **"Redimensionner"** pour appliquer
5. L'aperçu se met à jour automatiquement

#### Mode Couleur 1 cara
1. Sélectionner un **caractère prédéfini** ou entrer un caractère personnalisé
2. Choisir une **palette de couleur** (1, 8, 16, 32, 64, 128, 256 couleurs)
3. Si palette "1 couleur" : ajuster les valeurs RGB
4. Redimensionner si nécessaire

#### Mode Couleur multi
1. Choisir une **palette de couleur** (8, 16, 32, 64, 128, 256 couleurs)
2. Cocher **"Inverser les couleurs"** si nécessaire
3. Si palette "1 couleur" : ajuster les valeurs RGB
4. Redimensionner

### 4. Exporter le résultat

- Cliquer sur **"Enregistrer"**
- Choisir le format :
  - `.txt` : Texte brut (mode Simple uniquement)
  - `.html` : Page web avec couleurs
  - `.ansi` : Fichier terminal avec codes couleur
- Sélectionner l'emplacement

### Visualisation des fichiers ANSI

Les fichiers `.ansi` contiennent des codes d'échappement pour afficher les couleurs dans un terminal.

**Linux/macOS** :
```bash
cat fichier.ansi
```

**Windows (PowerShell 7+)** :
```powershell
Get-Content fichier.ansi
```

**Avec pagination** :
```bash
less -R fichier.ansi    # Linux/macOS
```

---

## Formats d'export

### Comparaison

| Format | Couleurs | Taille | Visualisation | Modification |
|--------|----------|--------|---------------|--------------|
| `.txt` | Non | Petite (Ko) | Éditeur texte | Facile |
| `.html` | Oui | Moyenne (Ko) | Navigateur web | HTML/CSS |
| `.ansi` | Oui | Petite (Ko) | Terminal | Difficile |

### Exemples d'utilisation

#### Partage sur réseaux sociaux
```bash
# Copier le contenu txt dans le presse-papier
xclip -selection clipboard < image.txt  # Linux
pbcopy < image.txt                      # macOS
```

#### Intégration dans documentation
- Format HTML : iframe ou inclusion directe
- Format ANSI : screenshots terminal ou asciinema

#### Signature email
Utiliser le format `.txt` en police monospace (Courier New)

---

## Remerciements

### Bibliothèques utilisées

- **Qt 6** : Framework d'interface graphique multiplateforme (LGPL v3)
  - [https://www.qt.io/](https://www.qt.io/)
  
- **stb_image.h** : Bibliothèque de chargement d'images (Public Domain)
  - Auteur : Sean Barrett
  - [https://github.com/nothings/stb](https://github.com/nothings/stb)

### Outils de développement

- **CMake** : Système de build multiplateforme
- **GCC/Clang/MSVC** : Compilateurs C++17
- **Git** : Gestion de versions

### Inspiration

Img2txt s'inspire des projets ASCII Art classiques tout en apportant :
- Support moderne des couleurs (HTML/ANSI)
- Interface graphique intuitive
- Multiples palettes artistiques
- Correction automatique du ratio d'aspect

---

## Licence

© 2026 Corentin. Tous droits réservés.

Ce logiciel utilise des bibliothèques tierces sous licences open source (Qt LGPL, stb Public Domain).

---

**Version** : 4.0  
**Date** : Février 2026  
**Contact** : [À compléter si souhaité]
