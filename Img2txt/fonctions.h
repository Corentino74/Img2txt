// Fichier de déclaration des fonctions et structures

// --- DÉPENDANCES
#pragma once
#ifndef FONCTIONS_H	// inclusion guard
#define FONCTIONS_H	//inclusion guard
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <algorithm>
#include <cctype>
#include "../stb_image.h" // bibliothèque pour la gestion des images


// --- STRUCTURES

// Structure pour stocker les données d'une image PGM
struct ImagePGM {
    int largeur;
    int hauteur;
	std::vector<uint8_t> pixels;    //vecteur d'octets pour stocker les valeurs de pixel
};

// Structure pour stocker les paramètres de la ligne de commande
struct Parametres {
	std::string input;  // nom du fichier d'entrée
	std::string output; // nom du fichier de sortie
    std::string palette;// nom du fichier de palette
	int width;          // largeur maximale de l'Ascii Art
    int height;        // hauteur maximale de l'Ascii Art
	bool help;          // flag pour afficher l'aide
};


// --- FONCTIONS
 
// Fonction générique pour charger une image (PGM, PNG, JPG)
ImagePGM chargerImage(const std::string& chemin);

// Fonction de lecture du fichier PGM
ImagePGM lireFichierPGM(const std::string& nomFichier);

// Fonction pour sauvegarder une image en format PGM
void sauvegarderEnPGM(const ImagePGM& image, const std::string& nomFichier);

// - Gestion ASCII ART -
// Transformation de l'image en ASCII art et retour sous forme de chaîne de caractères
std::string genererAsciiArt(const ImagePGM& image, const std::vector<std::string>& palette);

// Fonctions pour afficher l'ASCII art dans la console
void afficherAsciiArt(std::string img);

// Fonction pour sauvegarder l'ASCII art dans un fichier
void sauvegarderAsciiArt(const ImagePGM& image, const std::vector<std::string>& palette, const std::string& nomFichier);

// - Gestion des palettes et image -
// Fonction pour lire une palette depuis un fichier
std::vector<std::string> lirePalette(const std::string& nomFichier);

// passage à la palette par défaut
std::vector<std::string> getPaletteParDefaut();

// autres palettes possibles déjà dans le code
std::vector<std::string> getPaletteClassiqueEtendue();
std::vector<std::string> getPaletteBlocs();
std::vector<std::string> getPaletteNatureLumiere();
std::vector<std::string> getPaletteDetailsFins();
std::vector<std::string> getPaletteOmbreLumiere();
std::vector<std::string> getPaletteCyberpunk();
std::vector<std::string> getPaletteMedieval();
std::vector<std::string> getPaletteAquarelle();
std::vector<std::string> getPaletteHauteDefinition();
std::vector<std::string> getPaletteGradients();

// inversions des couleurs
ImagePGM inverserCouleurs(const ImagePGM& image);

// Fonctions pour le rééchantillonnage
ImagePGM redimensionnerImage(const ImagePGM& image, int nouvelleLargeur, int nouvelleHauteur);

// --- GESTION DES ARGUMENTS ---
// Fonctions pour la gestion des arguments
Parametres passerArguments(int argc, char* argv[]);

// afficher l'aide
void afficherAide();

// Fonctions utilitaire de calcul de moyenne des pixels
int calculerMoyenne(const std::vector<uint8_t>& pixels, int debut, int fin);

#endif // FONCTIONS_H#endif // FONCTIONS_H