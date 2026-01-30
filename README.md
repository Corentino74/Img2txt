# Img2txt 3.0 🎨
=======================

> **Img2txt** - Convertisseur d'images en art ASCII avec interface graphique Qt6 !
Cette version 3.5 propose une plus grandes gamme de palettes que la précédentes ! 
(mais à quand le support couleur ? Chut.)

## Fonctionnalités

- Interface graphique moderne avec Qt6
- Conversion d'images PNG/JPG en art ASCII
- Plein de palettes de caractères prédéfinies (ajoutez les vôtres si vous le voulez !)
- Fenêtre de calibrage pour prévisualiser le rendu
- Redimensionnement et ajustement du ratio
- Inversion du rendu (négatif) 
- Exportation en fichier texte
- Gestion de fichiers temporaires automatique
- Support de stb_image pour les formats d'images


## 📁 Structure du Projet

```
Img2txt/
├──  Img2txt/                     # Code source principal
│   ├── img2txt_20.cpp/.h         # Fenêtre principale de l'application
│   ├── FenetreCalibrage.cpp/.h   # Fenêtre de prévisualisation et calibrage
│   ├── FenetreChargement.cpp/.h  # Gestion du chargement d'images
│   └── fonctions.cpp/.h          # Fonctions utilitaires (conversion, palettes)
│
├── illustrations/                # Ressources graphiques (icônes UI)
│   ├── aide.png
│   ├── bin.png
│   ├── box.png
│   ├── collapse.png #(inutile)
│   ├── download.png
│   └── folder.png
│
├──    main.cpp                    # Point d'entrée de l'application
├──    stb_image.h                 # Bibliothèque de chargement d'images
├──    Img2txt_20.ui               # Interface Qt Designer
├──    Img2txt_20.qrc              # Fichier de ressources Qt
│
├── 🛠️ CMakeLists.txt              # Configuration du build CMake
├── 🛠️ CMakePresets.json          # Préréglages CMake
├── 🛠️ CMakeSettings.json         # Configuration CMake pour Visual Studio
│
├──     build_release.ps1           # Script de build et packaging Windows (PowerShell)
├──     build_release.sh            # Script de build Linux/Mac
│
├── 📖 README.md                   # Ce fichier
└── 📋 LICENSE                     # Licence du projet
```

---

## Installation et Compilation

```powershell
# Configuration
cmake -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.10.1/msvc2022_64" -DCMAKE_BUILD_TYPE=Release

# Compilation
cmake --build build --config Release

# L'exécutable se trouve dans build/Release/Img2txt_2_0.exe
```
OU

- Téléchargez l'archive/release -> Attention à votre OS
- Décompressez
- Un simple double-clique devrait suffire

## Guide d'utilisation

1. Lancez **Img2txt.exe**
2. Cliquez sur "Charger une image" 
3. Ajustez les paramètres (palette, taille, ratio)
4. Utilisez la fenêtre de calibrage pour prévisualiser
5. Exportez le résultat en fichier texte



## Licence

Disponible dans le fichier [LICENSE](LICENSE)


## Remerciements
- **stb_image** : Bibliothèque de chargement d'images de Sean Barrett
- **Qt Project** : Framework Qt6


## Contact
Pour toute question : corentinchitwood@gmail.com

<p align="center">
  Fait avec amour et beaucoup de caractères ASCII !
</p>
</p>

                              ???_~>i+?-+>l;,,,,,:;;;
                            +]-?{}]-<>i>>l;,,^^``''`,;
                          >><_?]_?->!I;;;II:,"^``'''''',
                        !-<>+-_++~!;,",,:;ll;:,"``````'^;
                        ;i__<}?+>l;!li<-[(trf/|1>;,^```'`;
                        :,;!<~--+>+}juXUQ0QJYzvxt]il:``''^
                        !+|nucvvcJLQ0ZZZO0LJUYcnxr{il;,"^^
                        i?cQ0OZmZZmwwwwqqmOQCCUcnjt{i;:,:",
                        l/LZwqdbbbddddddqw0LCCJzrt(?l,,^``"
                        (LwqqqddddbbdqmQLCQ0QCXj|({+;,"``"l
                        _Lww0CL0ZZZOLXf{~ii+}}-?1)()+:,"^>~
                        it1?+  [)xYUn{---[~,Ii+]])\\};l)/]+-
                         f1~-I,~?/0dJjnvuvunj||rucuj|-+1[j/|[
                         UcxuurxL0wdUtzJOOZOO00QCYvf|}+)-+\t~
                         QwdbbbbpOqwYxjXmqppppmQJXn\1]!)fff(
                        iQpdbpOzQ0qmUXvr(uvJ0QQXnf\){-!}tjj>
                         r0ZQx)YccYcf]<>)xUXjxcu/){}]+l+f)
                          nY/\YJvf1{]-1|/fjf\\\u|{}[]~;;
                           rrf(\)}{}}[[{}}tzXzUx{}[?_!:
                             vcUUXzYJcnunuzXXvj([[-~ii-<
                              fcJYzujjuczzYzx\[--+<<+_[-
                              i[nC0QLLLUYcnt)->i>_??])\
                               l<1nvuxrxjt{?+i<?]?]1\ff\>
                                 i--?--++~~+-????}|fjjjt{!
                                  ixr/)_~_-?--?{(/frxxxj\\)r\!
                                  nvxrxnxjffffrvYUUUYcnffrLLOmOQv~
                                 I)CCYununnnnnvYJCLLJYzvxX0OqbbppqZZj+
                              1whkcJLQCzccvvvczUJCCJYYXCZ0Zbhkbddpddbdpmc]!
                            OkhkahrzUCLJXccczYYUUJJYJmmZmdhhbkbdddbbbbhhkbbkbC
                        udahaahhahk/YJJJYccvvczYCZqwwwqbakdkkbdbdbbbkkhhhaao*ooakpO
                   w*##****ooaaakddpqqqmZmpdqqwwwpkahkkkkkbkdbdkbkbhh**o*#*oooooakp0_
               xk*####o##M###*oaoabpqwmmwwqpdbkhhbbhhhkkhkkbkbkbkhha**#*#**#**ohahkkbO
              o#####*#o##M##M######aooo**oakdbbbbhhhhhbhhbbdkbbbbhha#***###M##aka#*ohkd
           :J##M####**o*##M############***##*aahahhhhbbkkkkbkkkhbaao##oo#####ohbo###okdb
         IO#######**oo**######**#******#**ooo*o*oaahbkkkhkdkkhhbhao##oao###*abha*##*hwZp
        i*########*ooo*M#M##****o****oooooooo*###ohhhaaoakkhaakhao*#*oo##*oadhao##*ap0Zb
       "X*#######oo*ho###M##******oo*oooaoo*##*ookhko**ohko#ohao*###oa*#*oabpho*#*okwZqk
       |o*##M##*o***aa#####o**#****o*o****###**hkka*##*aka**ooa*##*aao##*akqdo**ooakqwba
      ;a*ooo##**oho*oa##*##oooo#*ooo*o**###**ohkbao*#*aha**ooa*#**ohh*##ohpmb#oaaoaakdpw
      {###ooo**ahh*#*oo####oooooo*oo**#*##*ohkdhho*##*hhao*oaooo*aaao*#ohdZqhoooooohpCUq {a***#ooahkko*ooo*##*oaoo**oo**#***ohhdbka*###*aoaoooooo#*aaka*#*abwOpoao**ohqCYZk