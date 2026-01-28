// Fichier de définitions des fonctions et structures

#pragma once
#include "fonctions.h"


// Fonction pour gestion png

// Fonction utilitaire pour obtenir l'extension d'un fichier en minuscules
static std::string obtenirExtension(const std::string& chemin) {
	const auto pos = chemin.find_last_of('.');
	if (pos == std::string::npos) return {};
	std::string ext = chemin.substr(pos + 1);
	std::transform(ext.begin(), ext.end(), ext.begin(), 
		[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
	return ext;
}

// Fonction générique pour charger une image (PGM, PNG, JPG)
ImagePGM chargerImage(const std::string& chemin) {
	const std::string ext = obtenirExtension(chemin);
	
	if (ext == "pgm") {
		return lireFichierPGM(chemin);
	}
	
	if (ext == "png" || ext == "jpg" || ext == "jpeg") {
		// Chargement avec stb_image
		int w = 0, h = 0, comp = 0;
		unsigned char* data = stbi_load(chemin.c_str(), &w, &h, &comp, 0);
		
		if (!data) {
			throw std::runtime_error("Impossible de charger l'image : " + chemin);
		}
		
		ImagePGM img;
		img.largeur = w;
		img.hauteur = h;
		img.pixels.resize(static_cast<size_t>(w) * static_cast<size_t>(h));
		
		// Conversion en niveaux de gris si nécessaire
		for (int y = 0; y < h; ++y) {
			for (int x = 0; x < w; ++x) {
				const int idx = (y * w + x) * comp;
				uint8_t gray = 0;
				
				if (comp == 1) {
					// Déjà en niveaux de gris
					gray = data[idx];
				} else if (comp == 3 || comp == 4) {
					// RGB ou RGBA => conversion en niveaux de gris
					const unsigned char r = data[idx + 0];
					const unsigned char g = data[idx + 1];
					const unsigned char b = data[idx + 2];
					gray = static_cast<uint8_t>(0.299 * r + 0.587 * g + 0.114 * b);
				}
				
				img.pixels[static_cast<size_t>(y) * static_cast<size_t>(w) + static_cast<size_t>(x)] = gray;
			}
		}
		
		stbi_image_free(data);
		return img;
	}
	
	throw std::runtime_error("Format de fichier non supporté : " + chemin);
}

//Fonction de lecture du fichier PGM
ImagePGM lireFichierPGM(const std::string& nomFichier) { // Renvoie une structure ImagePGM et prend un nom de fichier en entrée
	ImagePGM image;										 // Création d'une structure ImagePGM pour stocker les données de l'image
	std::ifstream fichier(nomFichier, std::ios::binary); // Ouvrir en mode binaire => important pour les fichiers PGM

	if (!fichier.is_open()) {							 // en cas d'erreur, renvoie un message
		throw std::runtime_error("Impossible d'ouvrir le fichier " + nomFichier);
	}

	// LECTURE LIGNE PAR LIGNES DU FICHIER POUR VÉRIFICATIONS
	// 
	// Vérification du format PGM avec la présence de P5
	std::string ligne;
	std::getline(fichier, ligne); // Lire la première ligne
	if (ligne != "P5") {		// si ne contient pas P5, renvoie un message d'erreur
		throw std::runtime_error("Format de fichier non supporté. Attendu : P5");
	}

	// Lire les dimensions
	std::getline(fichier, ligne);	// Lire la ligne suivante => dimensions
	std::istringstream iss(ligne);	// Utiliser istringstream pour extraire les dimensions
	if (!(iss >> image.largeur >> image.hauteur)) {	// Extraction des dimensions
		throw std::runtime_error("Impossible de lire les dimensions de l'image.");
	}

	// Lire la valeur maximale (255)
	std::getline(fichier, ligne);
	if (ligne != "255") {
		throw std::runtime_error("Valeur maximale de pixel inattendue. Attendu : 255");
	}

	// Lire les données binaires
	image.pixels.resize(image.largeur * image.hauteur);	// redimensionnement du vecteur de pixels de l'objet "Struct Img PGM" pour stocker les données
	if (!fichier.read(reinterpret_cast<char*>(image.pixels.data()), image.largeur * image.hauteur)) {
		throw std::runtime_error("Erreur lors de la lecture des données binaires.");
	}
	return image;	// renvoie de la structure ImagePGM avec les donées lues
}

// Fonction pour sauvegarder une image en format PGM
void sauvegarderEnPGM(const ImagePGM& image, const std::string& nomFichier) {
	std::ofstream fichier(nomFichier, std::ios::binary);
	if (!fichier.is_open()) {
		throw std::runtime_error("Impossible de créer le fichier PGM : " + nomFichier);
	}

	// Écrire l'en-tête PGM
	fichier << "P5\n";                           // Format binaire PGM
	fichier << image.largeur << " " << image.hauteur << "\n"; // Dimensions
	fichier << "255\n";                          // Valeur maximale

	// Écrire les données binaires des pixels
	fichier.write(reinterpret_cast<const char*>(image.pixels.data()), image.largeur * image.hauteur);

	fichier.close();
}

// - Gestion ASCII ART 
// Transformation de l'image en ASCII art et retour sous forme de chaîne de caractères
std::string genererAsciiArt(const ImagePGM& image, const std::vector<std::string>& palette) { // prend en entrée une structure ImagePGM et une palette
	std::ostringstream oss;										// utilisation d'un flux de sortie en mémoire pour construire la chaîne de caractères
	uint8_t pixel;
	int index;
	const int paletteSize = static_cast<int>(palette.size());	// taille de la palette donné en arguments
	
	// Algorithme de conversion, la magie opère !
	for (int y = 0; y < image.hauteur; ++y) {					// boucle sur les lignes de l'image
		for (int x = 0; x < image.largeur; ++x) {				// boucle sur les colonnes de l'image
			pixel = image.pixels[y * image.largeur + x];		// récupération de la valeur du pixel à la position (x, y) courante	
			index = (pixel * (paletteSize - 1)) / 255;			// calcul de l'index dans la palette en fonction de la valeur du pixel (0-255)
			oss << palette[index];								// ajout du caractère correspondant de la palette au flux de sortie
		}
		oss << '\n';											// fin du calcul de cette ligne, retour et calcul de la suivante
	}
	return oss.str();											// renvoi de la chaîne de caractères complète
}

// Fonctions pour afficher l'ASCII art dans la console
void afficherAsciiArt(std::string img) {
	std::cout << img;
}

// Fonction pour sauvegarder l'ascii art dans un fichier
void sauvegarderAsciiArt(const ImagePGM& image, const std::vector<std::string>& palette, const std::string& nomFichier) {
	// 1Ouverture du fichier en mode écriture
	std::ofstream fichier(nomFichier);
	if (!fichier.is_open()) {
		throw std::runtime_error("Impossible de créer le fichier de sortie.");
	}

	// 2Génération de l'Ascii Art
	std::string asciiArt = genererAsciiArt(image, palette);
	
	// 3Écriture dans le fichier
	fichier << asciiArt;

	// 4Fermeture du fichier
	fichier.close();
}

// Fonction pour inverser les couleurs de l'image
ImagePGM inverserCouleurs(const ImagePGM& image) {
	ImagePGM imageInversee = image; // Crée une copie de l'image originale
	for (size_t i = 0; i < imageInversee.pixels.size(); ++i) {
		imageInversee.pixels[i] = 255 - imageInversee.pixels[i]; // Inverse la valeur de chaque pixel
	}
	return imageInversee;
}

//GESTIONS DES PALETTES	
std::vector<std::string> getPaletteParDefaut() 
{
	return { "W", "w", "l", "i", ":", ",", ".", " " };
}
std::vector<std::string> getPaletteClassiqueEtendue() {
	return { "@", "#", "S", "%", "?", "*", "+", ";", ":", ",", ".", "`", " " };
}
std::vector<std::string> getPaletteBlocs() {
	return { "█", "▓", "▒", "░", " ", " " };
}
std::vector<std::string> getPaletteNatureLumiere() {
	return { "♠", "♣", "♥", "♦", "◊", "○", "●", "◌", " ", " " };
}
std::vector<std::string> getPaletteDetailsFins() {
	return { "@", "#", "S", "%", "*", "+", "=", ":", "-", ".", " " };
}
std::vector<std::string> getPaletteOmbreLumiere() {
	return { "▇", "▆", "▅", "▃", "▂", "▁", "▀", "·", " ", " " };
}
std::vector<std::string> getPaletteCyberpunk() {
	return { "■", "□", "◉", "○", "◍", "◎", "●", "◌", " ", " " };
}
std::vector<std::string> getPaletteMedieval() {
	return { "☠", "♔", "♖", "♗", "♘", "♙", "⚔", "⛓", " ", " " };
}
std::vector<std::string> getPaletteAquarelle() {
	return { "♫", "☼", "☁", "☂", "☺", "♀", "♂", "♻", " ", " " };
}
std::vector<std::string> getPaletteHauteDefinition() {
	return {
		"@", "▓", "▒", "░", "■", "●", "◘", "○", "◙", "◌",
		"◍", "☼", "☺", "♫", "♦", "♠", "♣", "♥", "◊", "♂",
		"♀", "∴", "∵", "≈", "≠", "≡", "≣", "⊕", "⊗", " "
	};
}
std::vector<std::string> getPaletteGradients() {
	return {
		"██", "▛▛", "▙▙", "▜▜", "▟▟", "▚▚", "▞▞", "▗▖", "▄▄", "▀▀",
		"▂▂", "▃▃", "▅▅", "▆▆", "▇▇", "■□", "●◌", "◉○", "◍◎", " "
	};
}

//déf fonction pour lire une pallette externe depuis un fichier
std::vector<std::string> lirePalette(const std::string& nomFichier) {
	std::ifstream fichier(nomFichier);
	if (!fichier.is_open()) {
		throw std::runtime_error("Impossible d'ouvrir le fichier de palette.");
	}

	std::vector<std::string> palette;
	std::string ligne;
	while (std::getline(fichier, ligne)) {
		if (!ligne.empty()) {
			palette.push_back(ligne);
		}
	}

	return palette;
}

//RECHANTILLONNAGE
//déf de la fonction pour redimensionner l'image
ImagePGM redimensionnerImage(const ImagePGM& image, int nouvelleLargeur, int nouvelleHauteur) {
	int oldWidth = image.largeur;
	int oldHeight = image.hauteur;

	// Calculer les ratios de redimensionnement
	double ratioWidth = static_cast<double>(oldWidth) / nouvelleLargeur;
	double ratioHeight = static_cast<double>(oldHeight) / nouvelleHauteur;

	ImagePGM nouvelleImage;
	nouvelleImage.largeur = nouvelleLargeur;
	nouvelleImage.hauteur = nouvelleHauteur;
	nouvelleImage.pixels.resize(nouvelleLargeur * nouvelleHauteur);

	for (int y = 0; y < nouvelleHauteur; ++y) {
		for (int x = 0; x < nouvelleLargeur; ++x) {
			// Calculer la zone correspondante dans l'image originale
			int startX = static_cast<int>(x * ratioWidth);
			int endX = static_cast<int>((x + 1) * ratioWidth);
			int startY = static_cast<int>(y * ratioHeight);
			int endY = static_cast<int>((y + 1) * ratioHeight);

			// Calculer la moyenne des pixels dans cette zone
			int sum = 0;
			int count = 0;
			for (int yy = startY; yy < endY && yy < oldHeight; ++yy) {
				for (int xx = startX; xx < endX && xx < oldWidth; ++xx) {
					sum += image.pixels[yy * oldWidth + xx];
					++count;
				}
			}
			nouvelleImage.pixels[y * nouvelleLargeur + x] = (count > 0) ? static_cast<uint8_t>(sum / count) : 0;
		}
	}
	return nouvelleImage;
}


//GESTION DES ARGUMENTS
// déf de la fonction pour passer les arguments de la ligne de commande
Parametres passerArguments(int argc, char* argv[]) {
	Parametres params;
	params.width = -1;
	params.height = -1;
	params.help = false;

	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];
		if (arg == "--input" && i + 1 < argc) {
			params.input = argv[++i];
		}
		else if (arg == "--output" && i + 1 < argc) {
			params.output = argv[++i];
		}
		else if (arg == "--palette" && i + 1 < argc) {
			params.palette = argv[++i];
		}
		else if (arg == "--width" && i + 1 < argc) {
			params.width = std::stoi(argv[++i]);
			if (params.width <= 0) {
				throw std::runtime_error("La largeur doit être un entier positif.");
			}
		}
		else if (arg == "--height" && i + 1 < argc) {
			params.height = std::stoi(argv[++i]);
			if (params.height <= 0) {
				throw std::runtime_error("La hauteur doit être un entier positif.");
			}
		}
		else if (arg == "--help") {
			params.help = true;
		}
	}

	return params;
}


// déf de la fonction pour afficher l'aide
void afficherAide() {
	std::cout << "Usage :\n"
		<< "pgm2txt [options]\n"
		<< "Options :\n"
		<< "--input fichier    Spécifie le fichier image à convertir\n"
		<< "--output fichier   Spécifie le nom du fichier texte qui contiendra l'Ascii Art\n"
		<< "--palette fichier  Spécifie un fichier texte contenant la palette de couleur Ascii\n"
		<< "--width nombre     Spécifie la largeur max de l'Ascii Art\n"
		<< "--height nombre    Spécifie la hauteur max de l'Ascii Art\n"
		<< "--help             Affiche cette aide\n";
}

// FONCTIONS UTILITAIRES
// déf de la fonction pour calculer la moyenne des pixels dans une plage donnée //A VERIFIER
int calculerMoyenne(const std::vector<uint8_t>& pixels, int debut, int fin) {
	if (debut < 0 || fin > static_cast<int>(pixels.size()) || debut >= fin) {
		throw std::out_of_range("Plage de pixels invalide");
	}
	int somme = 0;
	for (int i = debut; i < fin; ++i) {
		somme += pixels[i];
	}
	return somme / (fin - debut);
}