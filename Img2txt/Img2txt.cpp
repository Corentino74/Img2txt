// Img2txt.cpp : programme de conversion de fichier pgm en texte brut
#include "fonctions.h"
#include <iostream>
#include <limits>
#include <string>
#ifdef _WIN32
#include <windows.h> // Pour SetConsoleOutputCP
#endif

void afficherMenu() {
    std::cout << "\n=== Menu Ascii Art ===\n";
    std::cout << "1. Lire un fichier PGM\n";
    std::cout << "2. Charger une image PNG/JPG et convertir en PGM\n";
    std::cout << "3. Changer la palette\n";
    std::cout << "4. Enregistrer le résultat dans un fichier\n";
    std::cout << "5. Afficher l'aide\n";
    std::cout << "6. Redimensionner l'image\n";
    std::cout << "7. Inverser les couleurs de l'image\n";
    std::cout << "8. Afficher le résultat dans la console\n";
    std::cout << "9. Quitter\n";
    std::cout << "Choix : ";
}

int main(int argc, char* argv[]) {
    // Configuration pour afficher correctement les caractères spéciaux sous Windows
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    try {
        Parametres params = passerArguments(argc, argv);
        if (params.help) {
            afficherAide();
            return 0;
        }

        // Variables globales pour le menu
        ImagePGM image;
        std::vector<std::string> palette = getPaletteParDefaut();
        std::string fichierPGM;
        std::string fichierSortie;

        // Si un fichier est spécifié en argument, le charger directement
        if (!params.input.empty()) {
            try {
                image = lireFichierPGM(params.input);
                fichierPGM = params.input;
                if (!params.output.empty()) {
                    sauvegarderAsciiArt(image, palette, params.output);
                    return 0;
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Erreur : " << e.what() << std::endl;
                return 1;
            }
        }

        // Boucle du menu interactif
        while (true) {
            afficherMenu();
            std::string input;
            std::getline(std::cin, input);

            try {
                int choix = std::stoi(input);

                switch (choix) {
                case 1: {
                    std::cout << "Nom du fichier PGM : ";
                    std::getline(std::cin, fichierPGM);
                    try {
                        image = lireFichierPGM(fichierPGM);
                        std::cout << "Image chargée avec succès !\n";
                    }
                    catch (const std::exception& e) {
                        std::cerr << "Erreur : " << e.what() << std::endl;
                        break;
                    }
                    break;
                }
                case 2: {
                    std::cout << "Nom du fichier PNG/JPG : ";
                    std::getline(std::cin, fichierPGM);
                    try {
                        image = chargerImage(fichierPGM);
                        std::cout << "Image chargée avec succès !\n";
                        
                        // Proposer de sauvegarder en PGM
                        std::cout << "Voulez-vous sauvegarder cette image en PGM ? (o/n) : ";
                        std::string reponse;
                        std::getline(std::cin, reponse);
                        if (reponse == "o" || reponse == "O") {
                            std::cout << "Nom du fichier PGM de sortie : ";
                            std::getline(std::cin, fichierSortie);
                            try {
                                sauvegarderEnPGM(image, fichierSortie);
                                std::cout << "Image sauvegardée en PGM : " << fichierSortie << "\n";
                            }
                            catch (const std::exception& e) {
                                std::cerr << "Erreur : " << e.what() << std::endl;
                                break;
                            }
                        }
                    }
                    catch (const std::exception& e) {
                        std::cerr << "Erreur : " << e.what() << std::endl;
                        break;
                    }
                    break;
                }
                case 3: {
                    std::cout << "\n=== Choix de la Palette ===\n";
                    std::cout << "1. Palette par défaut\n";
                    std::cout << "2. Classique Étendue\n";
                    std::cout << "3. Blocs\n";
                    std::cout << "4. Nature/Lumière\n";
                    std::cout << "5. Détails fins\n";
                    std::cout << "6. Ombre/Lumière\n";
                    std::cout << "7. Cyberpunk\n";
                    std::cout << "8. Médiéval\n";
                    std::cout << "9. Aquarelle\n";
					std::cout << "10. Haute Définition\n";
					std::cout << "11. Gradients\n";
                    std::cout << "12. Charger depuis un fichier\n";
                    std::cout << "Choix : ";
                    std::getline(std::cin, input);
                    int choixPalette = std::stoi(input);

                    switch (choixPalette) {
                    case 1: palette = getPaletteParDefaut(); break;
                    case 2: palette = getPaletteClassiqueEtendue(); break;
                    case 3: palette = getPaletteBlocs(); break;
                    case 4: palette = getPaletteNatureLumiere(); break;
                    case 5: palette = getPaletteDetailsFins(); break;
                    case 6: palette = getPaletteOmbreLumiere(); break;
                    case 7: palette = getPaletteCyberpunk(); break;
                    case 8: palette = getPaletteMedieval(); break;
                    case 9: palette = getPaletteAquarelle(); break;
                    case 10: palette = getPaletteHauteDefinition(); break;
                    case 11: palette = getPaletteGradients(); break;
                    case 12: {
                        std::string fichierPalette;
                        std::cout << "Nom du fichier de palette : ";
                        std::getline(std::cin, fichierPalette);
                        try {
                            palette = lirePalette(fichierPalette);
                        }
                        catch (const std::exception& e) {
                            std::cerr << "Erreur : " << e.what() << std::endl;
                            break;
                        }
                        break;
                    }
                    default: std::cout << "Choix invalide.\n"; break;
                    }
                    std::cout << "Palette chargée avec succès !\n";
                    break;
                }
                case 4: {
                    if (image.pixels.empty()) {
                        std::cout << "Aucune image chargée. Chargez une image d'abord (option 1 ou 2).\n";
                        break;
                    }
                    std::cout << "Nom du fichier de sortie : ";
                    std::getline(std::cin, fichierSortie);
                    try {
                        sauvegarderAsciiArt(image, palette, fichierSortie);
                        std::cout << "Ascii Art enregistré.\n";
                    }
                    catch (const std::exception& e) {
                        std::cerr << "Erreur : " << e.what() << std::endl;
                        break;
                    }
                    break;
                }
                case 5: {
                    afficherAide();
                    break;
                }
                case 6: {
                    if (image.pixels.empty()) {
                        std::cout << "Aucune image chargée. Chargez une image d'abord (option 1 ou 2).\n";
                        break;
                    }
                    std::cout << "Nouvelle largeur (0 pour conserver) : ";
                    std::getline(std::cin, input);
                    int newWidth = std::stoi(input);
                    std::cout << "Nouvelle hauteur (0 pour conserver) : ";
                    std::getline(std::cin, input);
                    int newHeight = std::stoi(input);

                    if (newWidth == 0) newWidth = image.largeur;
                    if (newHeight == 0) newHeight = image.hauteur;
                    image = redimensionnerImage(image, newWidth, newHeight);
                    std::cout << "Image redimensionnée avec succès !\n";
                    break;
                }
                case 7: {
                    if (image.pixels.empty()) {
                        std::cout << "Aucune image chargée. Chargez une image d'abord (option 1 ou 2).\n";
                        break;
                    }
                    image = inverserCouleurs(image);
                    std::cout << "Couleurs inversées avec succès !\n";
                    break;
                }
                case 8: {
                    if (image.pixels.empty()) {
                        std::cout << "Aucune image chargée. Chargez une image d'abord (option 1 ou 2).\n";
                        break;
                    }
                    afficherAsciiArt(genererAsciiArt(image, palette));
                    break;
                }
                case 9: {
                    std::cout << "Au revoir !\n";
                    exit(0);
                }
                default: {
                    std::cout << "Choix invalide.\n";
                    break;
                }
                }
            }
            catch (...) {
                std::cout << "Saisie invalide. Veuillez entrer un nombre.\n";
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Erreur : " << e.what() << std::endl;
        return 1;
    }
    return 0;
}