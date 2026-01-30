#pragma once
// Fichier de déclaration des fonctions et structures

// --- DEPENDANCES

#include <vector>
#include <string>
#include <fstream>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <algorithm>
#include <cctype>
#include <stb_image.h>  // CMake s'occupe de trouver le fichier

// --- STRUCTURES

// Structure pour stocker les données d'une image PGM
struct ImagePGM {
	int largeur;
	int hauteur;
	std::vector<uint8_t> pixels;    //vecteur d'octets pour stocker les valeurs de pixel
};

// Structure pour stocker les param�tres de la ligne de commande
struct Parametres {
	std::string input;  // nom du fichier d'entr�e
	std::string output; // nom du fichier de sortie
	std::string palette;// nom du fichier de palette
	int width;          // largeur maximale de l'Ascii Art
	int height;        // hauteur maximale de l'Ascii Art
	bool help;          // flag pour afficher l'aide
};


// --- FONCTIONS

// Fonction g�n�rique pour charger une image (PGM, PNG, JPG)
ImagePGM chargerImage(const std::string& chemin);

// Fonction de lecture du fichier PGM
ImagePGM lireFichierPGM(const std::string& nomFichier);

// Fonction pour sauvegarder une image en format PGM
void sauvegarderEnPGM(const ImagePGM& image, const std::string& nomFichier);

// - Gestion ASCII ART -
// Transformation de l'image en ASCII art et retour sous forme de cha�ne de caract�res
std::string genererAsciiArt(const ImagePGM& image, const std::vector<std::string>& palette);

// Fonctions pour afficher l'ASCII art dans la console
void afficherAsciiArt(std::string img);

// Fonction pour sauvegarder l'ASCII art dans un fichier
void sauvegarderAsciiArt(const ImagePGM& image, const std::vector<std::string>& palette, const std::string& nomFichier);

// - Gestion des palettes et image -
// Fonction pour lire une palette depuis un fichier
std::vector<std::string> lirePalette(const std::string& nomFichier);

// passage a la palette par d�faut
std::vector<std::string> getPaletteParDefaut();

// autres palettes possibles d�j� dans le code
std::vector<std::string> getPaletteClassiqueEtendue();
std::vector<std::string> getPaletteBlocs();
std::vector<std::string> getPaletteNatureLumiere();
std::vector<std::string> getPaletteDetailsFins();
std::vector<std::string> getPaletteOmbreLumiere();
std::vector<std::string> getPaletteCyberpunk();
std::vector<std::string> getPaletteMedieval();
std::vector<std::string> getPaletteAquarelle();
std::vector<std::string> getPaletteSaturation();
std::vector<std::string> getPaletteGradients(); 
std::vector<std::string> getPalette2Bit();
std::vector<std::string> getPaletteNormale();
std::vector<std::string> getPaletteLettresSeules();
std::vector<std::string> getPaletteDetourage();
std::vector<std::string> getPaletteReflet();
std::vector<std::string> getPalettePoints();
std::vector<std::string> getPaletteLignes();
std::vector<std::string> getPaletteStippling();
std::vector<std::string> getPaletteDithering();
std::vector<std::string> getPaletteMinimaliste();
std::vector<std::string> getPaletteMonospace();
//std::vector<std::string> getPaletteLines(); //TODO

// inversions des couleurs
ImagePGM inverserCouleurs(const ImagePGM& image);

// Fonctions pour le reechantillonnage
ImagePGM redimensionnerImage(const ImagePGM& image, int nouvelleLargeur, int nouvelleHauteur);