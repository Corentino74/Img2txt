// Implémentation des fonctions de conversion d'images en ASCII
#include "fonctions.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


// Obtenir l'extension d'un fichier en minuscules
static std::string obtenirExtension(const std::string& chemin) {
	const auto pos = chemin.find_last_of('.');
	if (pos == std::string::npos) return {};
	std::string ext = chemin.substr(pos + 1);
	std::transform(ext.begin(), ext.end(), ext.begin(),
		[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
	return ext;
}

// Fonction générique pour charger une image (PGM, PNG, JPG, JPEG)	// ajout de webp ?
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
				}
				else if (comp == 3 || comp == 4) {
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
	std::getline(fichier, ligne);			 // Lire la ligne suivante => dimensions
	std::istringstream iss(ligne);					// Utiliser istringstream pour extraire les dimensions
	if (!(iss >> image.largeur >> image.hauteur)) {		// Extraction des dimensions
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

	// Algorithme de conversion avec correction du ratio
	// On parcourt l'image en sautant une ligne sur deux (step = 2) pour compenser le ratio 2:1 des caractères
	for (int y = 0; y < image.hauteur; y += 2) {				// corriger l'étirement vertical
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

// Palette Normale (anciennement Défaut)
std::vector<std::string> getPaletteNormale() {
	return { "W", "w", "l", "i", ":", ",", "."};
}

// Palette Classique
std::vector<std::string> getPaletteParDefaut()
{
	return { "@", "#", "S", "%", "?", "*", "+", ";", ":", ",", ".", " " };
}

// Palette classique étendue - Défaut amélioré
std::vector<std::string> getPaletteClassiqueEtendue() {
	return { "@", "#", "S", "%", "?", "*", "+", ";", ":", ",", ".", "`", " " };
}

// Palette Blocs - Parfaite pour le pixel art et les dégradés
std::vector<std::string> getPaletteBlocs() {
	return { "█", "▓", "▒", "░", " " };
}

// Palette Clair Obscur - Dégradés progressifs
std::vector<std::string> getPaletteOmbreLumiere() {
	return { "█", "▇", "▆", "▅", "▄", "▃", "▂", "▁", "·", " " };
}

// Palette Nature/Lumière - Symboles organiques
std::vector<std::string> getPaletteNatureLumiere() {
	return { "●", "◉", "○", "◌", "◊", "♦", "♥", "♠", "♣", " " };
}

// Palette Médiéval - Style textuel ancien
std::vector<std::string> getPaletteMedieval() {
	return { "#", "X", "x", "+", "=", "-", "~", ":", ".", " " };
}

// Palette Détails Fins - Bonne pour les portraits
std::vector<std::string> getPaletteDetailsFins() {
	return { "@", "#", "S", "%", "?", "*", "+", "=", "-", ":", ".", " " };
}

// Palette Aquarelle - Symboles légers
std::vector<std::string> getPaletteAquarelle() {
	return { "o", "O", "°", "º", "*", "·", "¨", "˙", ".", " " };
}

// Palette Gradients - Dégradés
std::vector<std::string> getPaletteGradients() {
	return { "█", "▓", "▒", "░", "▄", "▀", "■", "□", "●", "○", "·", " " };
}

// Palette Cyberpunk - Formes géométriques
std::vector<std::string> getPaletteCyberpunk() {
	return { "■", "□", "●", "○", "◉", "◌", "◍", "◎", "▪", "▫", " " };
}

// Palette 2-Bit - Minimaliste (noir et blanc pur)
std::vector<std::string> getPalette2Bit() {
	return { "█", " " };
}

// Palette Haute Définition - Grande gamme ASCII
std::vector<std::string> getPaletteSaturation() {
	return {
		"$", "@", "B", "%", "8", "&", "W", "M", "#", "*", "o", "a", "h",
		"k", "b", "d", "p", "q", "w", "m", "Z", "O", "0", "Q", "L", "C",
		"J", "U", "Y", "X", "z", "c", "v", "u", "n", "x", "r", "j", "f",
		"t", "/", "\\", "|", "(", ")", "1", "{", "}", "[", "]", "?", "-",
		"_", "+", "~", "<", ">", "i", "!", "l", "I", ";", ":", ",", "\"",
		"^", "`", "'", ".", " "
	};
}

// Palette Lettres Seules - Uniquement des lettres majuscules et minuscules
std::vector<std::string> getPaletteLettresSeules() {
	return { "M", "W", "N", "H", "K", "Q", "D", "B", "R", "A", "G", "m", "w", "n", "h", "k", "q", "d", "b", "r", "a", "g", "o", "e", "s", "c", "v", "x", "z", "u", "t", "i", "l", "j", " " };
}

// Palette Détourage - Supprime le fond blanc (commence par espace)
std::vector<std::string> getPaletteDetourage() {
	return { " ", ".", ":", ",", ";", "+", "=", "/", "%", "$", "M", "#" };
}

// Palette Reflet - Effet miroir/reflet pour les contours
std::vector<std::string> getPaletteReflet() {
	return { "◉", "◎", "○", "◦", "•", "·", "˙", "°", "¸", "'", "`", " " };
}

// Palette Points - Style dithering/tramage avec points
std::vector<std::string> getPalettePoints() {
	return { "█", "▓", "▒", "░", "▦", "▣", "▪", "▫", "•", "·", "˙", "·", " " };
}

// Palette Lignes - Style rétro/Atari avec lignes horizontales
std::vector<std::string> getPaletteLignes() {
	return { "█", "▀", "▄", "▐", "▌", "▖", "▗", "▘", "▝", "-", "_", " " };
}

// Palette Stippling - Technique de pointillisme
std::vector<std::string> getPaletteStippling() {
	return { "█", "●", "○", "◦", "•", "∙", "·", "˙", "·", ".", "'", " " };
}

// Palette Dithering - Tramage classique
std::vector<std::string> getPaletteDithering() {
	return { "█", "▓", "▒", "░", "▙", "▟", "▖", "▗", "▘", "▝", "·", " " };
}

// Palette Minimaliste - Très simple, grandes zones
std::vector<std::string> getPaletteMinimaliste() {
	return { "█", "▓", "-", ".", " " };
}

// Palette Monospace - Caractères de largeur fixe
std::vector<std::string> getPaletteMonospace() {
	return { "@", "#", "8", "&", "0", "X", "x", "+", "=", "-", ":", ".", " " };
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

// == SECTION SUPPORT COULEUR (V4.0) ==

// Chargement d'image en mode couleur (conserve RGB)
Image chargerImageCouleur(const std::string& chemin, bool conserverCouleur) {
	const std::string ext = obtenirExtension(chemin);
	
	// Si c'est un PGM, charger en monochrome
	if (ext == "pgm") {
		ImagePGM pgm = lireFichierPGM(chemin);
		return Image::fromImagePGM(pgm);
	}
	
	// Charger avec stb_image
	int w = 0, h = 0, comp = 0;
	unsigned char* data = stbi_load(chemin.c_str(), &w, &h, &comp, 0);
	
	if (!data) {
		throw std::runtime_error("Impossible de charger l'image : " + chemin);
	}
	
	Image img;
	img.largeur = w;
	img.hauteur = h;
	img.estCouleur = conserverCouleur;
	
	if (conserverCouleur) {
		// Stocker en RGB
		img.pixelsRGB.resize(static_cast<size_t>(w) * static_cast<size_t>(h));
		
		for (int y = 0; y < h; ++y) {
			for (int x = 0; x < w; ++x) {
				const int idx = (y * w + x) * comp;
				uint8_t r = 0, g = 0, b = 0;
				
				if (comp == 1) {
					r = g = b = data[idx];
				} else if (comp == 2) {
					r = g = b = data[idx];
				} else if (comp >= 3) {
					r = data[idx];
					g = data[idx + 1];
					b = data[idx + 2];
				}
				
				img.pixelsRGB[y * w + x] = std::make_tuple(r, g, b);
			}
		}
	} else {
		// Convertir en niveaux de gris
		img.pixelsGris.resize(static_cast<size_t>(w) * static_cast<size_t>(h));
		
		for (int y = 0; y < h; ++y) {
			for (int x = 0; x < w; ++x) {
				const int idx = (y * w + x) * comp;
				uint8_t gray = 0;
				
				if (comp == 1) {
					gray = data[idx];
				} else if (comp == 2) {
					gray = data[idx];
				} else if (comp >= 3) {
					const uint8_t r = data[idx];
					const uint8_t g = data[idx + 1];
					const uint8_t b = data[idx + 2];
					gray = static_cast<uint8_t>(0.299 * r + 0.587 * g + 0.114 * b);
				}
				
				img.pixelsGris[y * w + x] = gray;
			}
		}
	}
	
	stbi_image_free(data);
	return img;
}

// Calcul de la luminosité d'une couleur RGB (formule ITU-R BT.709)
float calculerLuminosite(uint8_t r, uint8_t g, uint8_t b) {
	return (0.2126f * r + 0.7152f * g + 0.0722f * b) / 255.0f;
}

// Trouver la couleur la plus proche dans une palette (distance euclidienne RGB)
int trouverCouleurProche(uint8_t r, uint8_t g, uint8_t b, const PaletteCouleur& palette) {
	if (palette.couleurs.empty()) {
		return 0;
	}
	
	int minDist = INT_MAX;
	int indexMin = 0;
	
	for (size_t i = 0; i < palette.couleurs.size(); ++i) {
		const auto& c = palette.couleurs[i];
		int dr = r - c.r;
		int dg = g - c.g;
		int db = b - c.b;
		int dist = dr * dr + dg * dg + db * db;
		
		if (dist < minDist) {
			minDist = dist;
			indexMin = static_cast<int>(i);
		}
	}
	
	return indexMin;
}

// Palette 8 couleurs ANSI de base
PaletteCouleur getPaletteCouleur8() {
	PaletteCouleur palette;
	palette.mode = ModeRendu::COULEUR_8;
	palette.nom = "8 couleurs ANSI";
	
	palette.couleurs = {
		CouleurRGB(0, 0, 0, "Noir"),
		CouleurRGB(170, 0, 0, "Rouge"),
		CouleurRGB(0, 170, 0, "Vert"),
		CouleurRGB(170, 85, 0, "Jaune"),
		CouleurRGB(0, 0, 170, "Bleu"),
		CouleurRGB(170, 0, 170, "Magenta"),
		CouleurRGB(0, 170, 170, "Cyan"),
		CouleurRGB(170, 170, 170, "Blanc")
	};
	
	palette.caracteres = {
		"█", "@", "#", "S", "%", "*", "+", "."
	};
	
	return palette;
}

// Palette 16 couleurs ANSI (8 + brillantes)
PaletteCouleur getPaletteCouleur16() {
	PaletteCouleur palette;
	palette.mode = ModeRendu::COULEUR_16;
	palette.nom = "16 couleurs ANSI";
	
	palette.couleurs = {
		// Couleurs normales
		CouleurRGB(0, 0, 0, "Noir"),
		CouleurRGB(170, 0, 0, "Rouge"),
		CouleurRGB(0, 170, 0, "Vert"),
		CouleurRGB(170, 85, 0, "Jaune"),
		CouleurRGB(0, 0, 170, "Bleu"),
		CouleurRGB(170, 0, 170, "Magenta"),
		CouleurRGB(0, 170, 170, "Cyan"),
		CouleurRGB(170, 170, 170, "Gris"),
		// Couleurs brillantes
		CouleurRGB(85, 85, 85, "Gris Foncé"),
		CouleurRGB(255, 85, 85, "Rouge Vif"),
		CouleurRGB(85, 255, 85, "Vert Vif"),
		CouleurRGB(255, 255, 85, "Jaune Vif"),
		CouleurRGB(85, 85, 255, "Bleu Vif"),
		CouleurRGB(255, 85, 255, "Magenta Vif"),
		CouleurRGB(85, 255, 255, "Cyan Vif"),
		CouleurRGB(255, 255, 255, "Blanc")
	};
	
	palette.caracteres = {
		"█", "▓", "▒", "░", "@", "#", "S", "%",
		"?", "*", "+", "=", "-", ":", ".", " "
	};
	
	return palette;
}

// Palette 1 couleur personnalisée (RGB)
PaletteCouleur getPaletteCouleur1(uint8_t r, uint8_t g, uint8_t b) {
	PaletteCouleur palette;
	palette.mode = ModeRendu::COULEUR_8;
	palette.nom = "Couleur unique";
	
	palette.couleurs.push_back(CouleurRGB(r, g, b, "Custom"));
	
	palette.caracteres = {
		"█", "▓", "▒", "░", "@", "#", "S", "%",
		"?", "*", "+", "=", "-", ":", ".", " "
	};
	
	return palette;
}

// Palette 32 couleurs (ANSI 16 + extensions)
PaletteCouleur getPaletteCouleur32() {
	PaletteCouleur palette;
	palette.mode = ModeRendu::COULEUR_16;
	palette.nom = "32 couleurs";
	
	// Récupérer les 16 de base
	auto p16 = getPaletteCouleur16();
	palette.couleurs = p16.couleurs;
	
	// Ajouter 16 couleurs intermédiaires
	palette.couleurs.push_back(CouleurRGB(128,   0,   0, "Maroon"));
	palette.couleurs.push_back(CouleurRGB(139,  69,  19, "SaddleBrown"));
	palette.couleurs.push_back(CouleurRGB(128, 128,   0, "Olive"));
	palette.couleurs.push_back(CouleurRGB(  0, 100,   0, "DarkGreen"));
	palette.couleurs.push_back(CouleurRGB( 72,  61, 139, "DarkSlateBlue"));
	palette.couleurs.push_back(CouleurRGB( 47,  79,  79, "DarkSlateGray"));
	palette.couleurs.push_back(CouleurRGB(  0, 128, 128, "Teal"));
	palette.couleurs.push_back(CouleurRGB( 70, 130, 180, "SteelBlue"));
	palette.couleurs.push_back(CouleurRGB(255, 140,   0, "DarkOrange"));
	palette.couleurs.push_back(CouleurRGB(255, 215,   0, "Gold"));
	palette.couleurs.push_back(CouleurRGB(173, 255,  47, "GreenYellow"));
	palette.couleurs.push_back(CouleurRGB(  0, 255, 127, "SpringGreen"));
	palette.couleurs.push_back(CouleurRGB(  0, 191, 255, "DeepSkyBlue"));
	palette.couleurs.push_back(CouleurRGB(138,  43, 226, "BlueViolet"));
	palette.couleurs.push_back(CouleurRGB(255,  20, 147, "DeepPink"));
	palette.couleurs.push_back(CouleurRGB(255, 182, 193, "LightPink"));
	
	palette.caracteres = {
		"█", "▓", "▒", "░", "@", "#", "S", "%",
		"?", "*", "+", "=", "-", ":", ".", " "
	};
	
	return palette;
}

// Palette 64 couleurs (cube RGB réduit - coûteux en calcul)
PaletteCouleur getPaletteCouleur64() {
	PaletteCouleur palette;
	palette.mode = ModeRendu::COULEUR_256;
	palette.nom = "64 couleurs (calcul intensif)";
	
	// Cube 4x4x4 = 64 couleurs
	for (int r = 0; r < 4; ++r) {
		for (int g = 0; g < 4; ++g) {
			for (int b = 0; b < 4; ++b) {
				uint8_t rv = static_cast<uint8_t>(r * 85);  // 0, 85, 170, 255
				uint8_t gv = static_cast<uint8_t>(g * 85);
				uint8_t bv = static_cast<uint8_t>(b * 85);
				palette.couleurs.push_back(CouleurRGB(rv, gv, bv));
			}
		}
	}
	
	palette.caracteres = {
		"█", "▓", "▒", "░", "@", "#", "S", "%",
		"?", "*", "+", "=", "-", ":", ".", " "
	};
	
	return palette;
}

// Palette 128 couleurs (cube RGB moyen - très coûteux)
PaletteCouleur getPaletteCouleur128() {
	PaletteCouleur palette;
	palette.mode = ModeRendu::COULEUR_256;
	palette.nom = "128 couleurs (très coûteux)";
	
	// ANSI 16 de base
	auto p16 = getPaletteCouleur16();
	palette.couleurs = p16.couleurs;
	
	// Cube 4x4x4 supplémentaire
	for (int r = 0; r < 4; ++r) {
		for (int g = 0; g < 4; ++g) {
			for (int b = 0; b < 4; ++b) {
				uint8_t rv = static_cast<uint8_t>(r * 85);
				uint8_t gv = static_cast<uint8_t>(g * 85);
				uint8_t bv = static_cast<uint8_t>(b * 85);
				palette.couleurs.push_back(CouleurRGB(rv, gv, bv));
			}
		}
	}
	
	// Niveaux de gris étendus (16 nuances)
	for (int i = 0; i < 16; ++i) {
		uint8_t gray = static_cast<uint8_t>(i * 17);  // 0, 17, 34..., 255
		palette.couleurs.push_back(CouleurRGB(gray, gray, gray));
	}
	
	// Couleurs intermédiaires supplémentaires
	for (int r = 1; r < 4; r += 2) {
		for (int g = 1; g < 4; g += 2) {
			for (int b = 1; b < 4; b += 2) {
				uint8_t rv = static_cast<uint8_t>(r * 42);  // Demi-tons
				uint8_t gv = static_cast<uint8_t>(g * 42);
				uint8_t bv = static_cast<uint8_t>(b * 42);
				palette.couleurs.push_back(CouleurRGB(rv, gv, bv));
			}
		}
	}
	
	palette.caracteres = {
		"█", "▓", "▒", "░", "@", "#", "S", "%",
		"?", "*", "+", "=", "-", ":", ".", " "
	};
	
	return palette;
}

// Palette 256 couleurs xterm (simplifiée pour performance)
PaletteCouleur getPaletteCouleur256() {
	PaletteCouleur palette;
	palette.mode = ModeRendu::COULEUR_256;
	palette.nom = "256 couleurs xterm";
	
	// 16 couleurs ANSI de base
	auto p16 = getPaletteCouleur16();
	palette.couleurs = p16.couleurs;
	
	// Cube de couleurs 6x6x6 (216 couleurs)
	for (int r = 0; r < 6; ++r) {
		for (int g = 0; g < 6; ++g) {
			for (int b = 0; b < 6; ++b) {
				uint8_t rv = r * 51;
				uint8_t gv = g * 51;
				uint8_t bv = b * 51;
				palette.couleurs.push_back(CouleurRGB(rv, gv, bv));
			}
		}
	}
	
	// Niveaux de gris (24 nuances)
	for (int i = 0; i < 24; ++i) {
		uint8_t gray = static_cast<uint8_t>(8 + i * 10);
		palette.couleurs.push_back(CouleurRGB(gray, gray, gray));
	}
	
	// Caractères variés
	palette.caracteres = {
		"█", "▓", "▒", "░", "▄", "▀", "■", "□",
		"●", "○", "@", "#", "S", "%", "?", "*",
		"+", "=", "-", ":", ".", " "
	};
	
	return palette;
}

// True Color (utilise quantification simple)
PaletteCouleur getPaletteTrueColor() {
	PaletteCouleur palette;
	palette.mode = ModeRendu::TRUE_COLOR;
	palette.nom = "True Color (16M)";
	
	// Pour True Color, on n'a pas besoin de palette prédéfinie
	// Les couleurs seront encodées directement en RGB
	palette.caracteres = {
		"█", "▓", "▒", "░", "@", "#", "S", "%",
		"?", "*", "+", ":", ".", " "
	};
	
	return palette;
}

// Génération ASCII art couleur
std::string genererAsciiArtCouleur(const Image& image, const PaletteCouleur& palette) {
	if (!image.estCouleur) {
		throw std::runtime_error("L'image doit être en mode couleur");
	}
	
	std::stringstream result;
	
	// Saut d'une ligne sur deux pour compenser le ratio 2:1 des caractères
	for (int y = 0; y < image.hauteur; y += 2) {
		for (int x = 0; x < image.largeur; ++x) {
			auto [r, g, b] = image.pixelsRGB[y * image.largeur + x];
			
			// Calculer la luminosité pour choisir le caractère
			float lum = calculerLuminosite(r, g, b);
			int charIndex = static_cast<int>(lum * (palette.caracteres.size() - 1));
			charIndex = std::min(charIndex, static_cast<int>(palette.caracteres.size()) - 1);
			
			// Trouver la couleur la plus proche
			int couleurIndex = 0;
			if (palette.mode != ModeRendu::TRUE_COLOR) {
				couleurIndex = trouverCouleurProche(r, g, b, palette);
				couleurIndex = std::min(couleurIndex, static_cast<int>(palette.couleurs.size()) - 1);
			}
			
			// Stocker les infos (on formatte plus tard selon le format)
			result << palette.caracteres[charIndex];
		}
		result << "\n";
	}
	
	return result.str();
}

// Génération ASCII art mono-caractère (mode "Couleur 1 cara")
std::string genererAsciiArtCouleurMonoCaractere(const Image& image, const PaletteCouleur& /* palette non utilisée ici, erreur sans ce commentaire ???s*/, const std::string& caractere) {
	if (!image.estCouleur) {
		throw std::runtime_error("L'image doit être en mode couleur");
	}
	
	// Utiliser un espace si caractère vide
	std::string charUtilise = caractere.empty() ? " " : caractere;
	
	std::stringstream result;
	
	// Saut d'une ligne sur deux pour compenser le ratio 2:1 des caractères
	for (int y = 0; y < image.hauteur; y += 2) {
		for (int x = 0; x < image.largeur; ++x) {
			// Utiliser toujours le même caractère, seule la couleur change
			result << charUtilise;
		}
		result << "\n";
	}
	
	return result.str();
}

// Export en HTML avec couleurs
void sauvegarderAsciiArtHTML(const Image& image, const PaletteCouleur& palette, 
                              const std::string& nomFichier, bool fondNoir) {
	if (!image.estCouleur) {
		throw std::runtime_error("L'image doit être en mode couleur pour export HTML");
	}
	
	std::ofstream fichier(nomFichier);
	if (!fichier.is_open()) {
		throw std::runtime_error("Impossible d'ouvrir le fichier : " + nomFichier);
	}
	
	// En-tête HTML
	fichier << "<!DOCTYPE html>\n";
	fichier << "<html>\n<head>\n";
	fichier << "    <meta charset=\"UTF-8\">\n";
	fichier << "    <title>ASCII Art - " << palette.nom << "</title>\n";
	fichier << "    <style>\n";
	fichier << "        body {\n";
	fichier << "            background: " << (fondNoir ? "#000000" : "#FFFFFF") << ";\n";
	fichier << "            margin: 0;\n";
	fichier << "            padding: 20px;\n";
	fichier << "            display: flex;\n";
	fichier << "            justify-content: center;\n";
	fichier << "            align-items: center;\n";
	fichier << "            min-height: 100vh;\n";
	fichier << "        }\n";
	fichier << "        pre {\n";
	fichier << "            font-family: 'Courier New', 'Consolas', monospace;\n";
	fichier << "            font-size: 8px;\n";
	fichier << "            line-height: 8px;\n";
	fichier << "            letter-spacing: 0;\n";
	fichier << "            margin: 0;\n";
	fichier << "            white-space: pre;\n";
	fichier << "        }\n";
	fichier << "    </style>\n";
	fichier << "</head>\n<body>\n<pre>";
	
	// Génération du contenu avec couleurs
	// Saut d'une ligne sur deux pour compenser le ratio 2:1 des caractères
	for (int y = 0; y < image.hauteur; y += 2) {
		for (int x = 0; x < image.largeur; ++x) {
			auto [r, g, b] = image.pixelsRGB[y * image.largeur + x];
			
			// Calculer luminosité pour caractère
			float lum = calculerLuminosite(r, g, b);
			int charIndex = static_cast<int>(lum * (palette.caracteres.size() - 1));
			charIndex = std::min(charIndex, static_cast<int>(palette.caracteres.size()) - 1);
			
			// Déterminer la couleur
			uint8_t cr = r, cg = g, cb = b;
			if (palette.mode != ModeRendu::TRUE_COLOR) {
				int couleurIndex = trouverCouleurProche(r, g, b, palette);
				couleurIndex = std::min(couleurIndex, static_cast<int>(palette.couleurs.size()) - 1);
				const auto& coul = palette.couleurs[couleurIndex];
				cr = coul.r;
				cg = coul.g;
				cb = coul.b;
			}
			
			// Écrire le caractère avec sa couleur
			fichier << "<span style=\"color:rgb(" << static_cast<int>(cr) << ","
			        << static_cast<int>(cg) << "," << static_cast<int>(cb) << ")\">"
			        << palette.caracteres[charIndex] << "</span>";
		}
		fichier << "\n";
	}
	
	// Pied de page
	fichier << "</pre>\n</body>\n</html>";
	fichier.close();
}

// Export en ANSI avec codes d'échappement
void sauvegarderAsciiArtANSI(const Image& image, const PaletteCouleur& palette, 
                              const std::string& nomFichier) {
	if (!image.estCouleur) {
		throw std::runtime_error("L'image doit être en mode couleur pour export ANSI");
	}
	
	std::ofstream fichier(nomFichier);
	if (!fichier.is_open()) {
		throw std::runtime_error("Impossible d'ouvrir le fichier : " + nomFichier);
	}
	
	// Génération avec codes ANSI
	// Saut d'une ligne sur deux pour compenser le ratio 2:1 des caractères
	for (int y = 0; y < image.hauteur; y += 2) {
		for (int x = 0; x < image.largeur; ++x) {
			auto [r, g, b] = image.pixelsRGB[y * image.largeur + x];
			
			// Calculer luminosité pour caractère
			float lum = calculerLuminosite(r, g, b);
			int charIndex = static_cast<int>(lum * (palette.caracteres.size() - 1));
			charIndex = std::min(charIndex, static_cast<int>(palette.caracteres.size()) - 1);
			
			// Déterminer la couleur
			uint8_t cr = r, cg = g, cb = b;
			if (palette.mode != ModeRendu::TRUE_COLOR) {
				int couleurIndex = trouverCouleurProche(r, g, b, palette);
				couleurIndex = std::min(couleurIndex, static_cast<int>(palette.couleurs.size()) - 1);
				const auto& coul = palette.couleurs[couleurIndex];
				cr = coul.r;
				cg = coul.g;
				cb = coul.b;
			}
			
			// Code ANSI pour True Color : \033[38;2;R;G;Bm
			fichier << "\033[38;2;" << static_cast<int>(cr) << ";"
			        << static_cast<int>(cg) << ";" << static_cast<int>(cb) << "m"
			        << palette.caracteres[charIndex];
		}
		fichier << "\033[0m\n";  // Reset couleur à la fin de la ligne
	}
	
	fichier.close();
}

// Redimensionnement d'image couleur (équivalent RGB de redimensionnerImage)
Image redimensionnerImageCouleur(const Image& image, int nouvelleLargeur, int nouvelleHauteur) {
	// Si l'image est en niveaux de gris, utiliser la fonction monochrome
	if (!image.estCouleur) {
		ImagePGM pgm = image.toImagePGM();
		ImagePGM redim = redimensionnerImage(pgm, nouvelleLargeur, nouvelleHauteur);
		return Image::fromImagePGM(redim);
	}
	
	int oldWidth = image.largeur;
	int oldHeight = image.hauteur;
	
	// Calculer les ratios de redimensionnement
	double ratioWidth = static_cast<double>(oldWidth) / nouvelleLargeur;
	double ratioHeight = static_cast<double>(oldHeight) / nouvelleHauteur;
	
	Image nouvelleImage;
	nouvelleImage.largeur = nouvelleLargeur;
	nouvelleImage.hauteur = nouvelleHauteur;
	nouvelleImage.estCouleur = true;
	nouvelleImage.pixelsRGB.resize(nouvelleLargeur * nouvelleHauteur);
	
	for (int y = 0; y < nouvelleHauteur; ++y) {
		for (int x = 0; x < nouvelleLargeur; ++x) {
			// Calculer la zone correspondante dans l'image originale
			int startX = static_cast<int>(x * ratioWidth);
			int endX = static_cast<int>((x + 1) * ratioWidth);
			int startY = static_cast<int>(y * ratioHeight);
			int endY = static_cast<int>((y + 1) * ratioHeight);
			
			// Calculer la moyenne des pixels RGB dans cette zone
			int sumR = 0, sumG = 0, sumB = 0;
			int count = 0;
			for (int yy = startY; yy < endY && yy < oldHeight; ++yy) {
				for (int xx = startX; xx < endX && xx < oldWidth; ++xx) {
					auto [r, g, b] = image.pixelsRGB[yy * oldWidth + xx];
					sumR += r;
					sumG += g;
					sumB += b;
					++count;
				}
			}
			
			uint8_t moyR = (count > 0) ? static_cast<uint8_t>(sumR / count) : 0;
			uint8_t moyG = (count > 0) ? static_cast<uint8_t>(sumG / count) : 0;
			uint8_t moyB = (count > 0) ? static_cast<uint8_t>(sumB / count) : 0;
			
			nouvelleImage.pixelsRGB[y * nouvelleLargeur + x] = std::make_tuple(moyR, moyG, moyB);
		}
	}
	
	return nouvelleImage;
}

// Fin du fichier fonctions.cpp