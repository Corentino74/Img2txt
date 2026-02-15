// Fichier de déclaration des fonctions et structures
#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <algorithm>
#include <cctype>
#include <tuple>
#include <cmath>
#include <stb_image.h>  // CMake s'occupe de trouver le fichier

// --- STRUCTURES

// Structure pour stocker les données d'une image PGM (niveaux de gris)
struct ImagePGM {
	int largeur;
	int hauteur;
	std::vector<uint8_t> pixels;    //vecteur d'octets pour stocker les valeurs de pixel
};

// Énumération pour les modes de rendu couleur
enum class ModeRendu {
	MONOCHROME,     // Mode actuel (niveaux de gris)
	COULEUR_8,      // 8 couleurs ANSI de base
	COULEUR_16,     // 16 couleurs ANSI (8 + brillantes)
	COULEUR_256,    // 256 couleurs xterm
	TRUE_COLOR      // RGB complet (16 millions)
};

// Structure pour représenter une couleur RGB
struct CouleurRGB {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	std::string nom;  // Nom de la couleur (optionnel)
	
	// Constructeurs
	CouleurRGB() : r(0), g(0), b(0), nom("") {}
	CouleurRGB(uint8_t r_, uint8_t g_, uint8_t b_, const std::string& nom_ = "")
		: r(r_), g(g_), b(b_), nom(nom_) {}
};

// Structure pour palette de couleurs avec caractères associés
struct PaletteCouleur {
	ModeRendu mode;
	std::vector<CouleurRGB> couleurs;
	std::vector<std::string> caracteres;  // Correspondance 1:1 avec couleurs
	std::string nom;  // Nom de la palette
	
	PaletteCouleur() : mode(ModeRendu::COULEUR_8), nom("") {}
};

// Structure unifiée pour images (remplace ImagePGM des anciennes versions)
struct Image {
	int largeur;
	int hauteur;
	bool estCouleur;  // true = RGB, false = niveaux de gris "classique"
	
	// Stockage des pixels selon le mode
	std::vector<uint8_t> pixelsGris;                                    // Mode monochrome
	std::vector<std::tuple<uint8_t, uint8_t, uint8_t>> pixelsRGB;     // Mode couleur
	
	// Constructeur par défaut
	Image() : largeur(0), hauteur(0), estCouleur(false) {}
	
	// Constructeur pour mode monochrome
	Image(int l, int h, const std::vector<uint8_t>& pixels)
		: largeur(l), hauteur(h), estCouleur(false), pixelsGris(pixels) {}
	
	// Constructeur pour mode couleur
	Image(int l, int h, const std::vector<std::tuple<uint8_t, uint8_t, uint8_t>>& pixels)
		: largeur(l), hauteur(h), estCouleur(true), pixelsRGB(pixels) {}
	
	// Conversion depuis ImagePGM
	static Image fromImagePGM(const ImagePGM& pgm) {
		return Image(pgm.largeur, pgm.hauteur, pgm.pixels);
	}
	
	// Conversion vers ImagePGM
	ImagePGM toImagePGM() const {
		ImagePGM pgm;
		pgm.largeur = largeur;
		pgm.hauteur = hauteur;
		pgm.pixels = pixelsGris;
		return pgm;
	}
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

// Fonction generique pour charger une image (PGM, PNG, JPG)
ImagePGM chargerImage(const std::string& chemin);

// Fonction de lecture du fichier PGM
ImagePGM lireFichierPGM(const std::string& nomFichier);

// Fonction pour sauvegarder une image en format PGM
void sauvegarderEnPGM(const ImagePGM& image, const std::string& nomFichier);

// - Gestion ASCII ART -
// Transformation de l'image en ASCII art et retour sous forme de chaine de caracteres
std::string genererAsciiArt(const ImagePGM& image, const std::vector<std::string>& palette);

// Fonctions pour afficher l'ASCII art dans la console
void afficherAsciiArt(std::string img);

// Fonction pour sauvegarder l'ASCII art dans un fichier
void sauvegarderAsciiArt(const ImagePGM& image, const std::vector<std::string>& palette, const std::string& nomFichier);

// - Gestion des palettes et image -
// Fonction pour lire une palette depuis un fichier
std::vector<std::string> lirePalette(const std::string& nomFichier);
// liste pallettes (Ajoutez ici les vôtres !)
std::vector<std::string> getPaletteParDefaut();
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

// inversions des couleurs
ImagePGM inverserCouleurs(const ImagePGM& image);

// Fonctions pour le reechantillonnage
ImagePGM redimensionnerImage(const ImagePGM& image, int nouvelleLargeur, int nouvelleHauteur);

// Redimensionnement pour images couleur
Image redimensionnerImageCouleur(const Image& image, int nouvelleLargeur, int nouvelleHauteur);

// --- FONCTIONS COULEUR (V4.0) ---

// Chargement d'images en mode couleur
Image chargerImageCouleur(const std::string& chemin, bool conserverCouleur = true);

// Palettes de couleurs prédéfinies
PaletteCouleur getPaletteCouleur1(uint8_t r, uint8_t g, uint8_t b);  // 1 couleur personnalisée
PaletteCouleur getPaletteCouleur8();    // 8 couleurs ANSI de base
PaletteCouleur getPaletteCouleur16();   // 16 couleurs ANSI
PaletteCouleur getPaletteCouleur32();   // 32 couleurs (palette étendue)
PaletteCouleur getPaletteCouleur64();   // 64 couleurs (coûteux)
PaletteCouleur getPaletteCouleur128();  // 128 couleurs (coûteux)
PaletteCouleur getPaletteCouleur256();  // 256 couleurs xterm (instable)
PaletteCouleur getPaletteTrueColor();   // RGB complet (16 millions de couleurs, très coûteux)

// Conversion d'images en ASCII art couleur
std::string genererAsciiArtCouleur(const Image& image, const PaletteCouleur& palette);

// Génération ASCII mono-caractère (mode "Couleur 1 cara")
std::string genererAsciiArtCouleurMonoCaractere(const Image& image, const PaletteCouleur& palette, const std::string& caractere);

// Fonctions utilitaires pour couleurs
int trouverCouleurProche(uint8_t r, uint8_t g, uint8_t b, const PaletteCouleur& palette); // Retourne l'index de la couleur la plus proche dans la palette
float calculerLuminosite(uint8_t r, uint8_t g, uint8_t b);								// Calcul de la luminosité d'une couleur (pour correspondance avec caractères)
CouleurRGB moyennerCouleur(const Image& image, int x, int y, int tailleBloc);	// Moyenne des couleurs dans un bloc (pour redimensionnement et correspondance avec caractères)	

// Sauvergardes sous formats HTML et ANSI avec couleurs
void sauvegarderAsciiArtHTML(const Image& image, const PaletteCouleur& palette, const std::string& nomFichier, bool fondNoir = true);
void sauvegarderAsciiArtANSI(const Image& image, const PaletteCouleur& palette, const std::string& nomFichier);