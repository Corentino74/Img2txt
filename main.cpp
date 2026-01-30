#include "Img2txt/img2txt_20.h"        // ← Chemin complet
#include "Img2txt/FenetreChargement.h" // ← Chemin complet
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // Configuration UTF-8
    app.setApplicationName("Img2Txt 3.0");
    app.setOrganizationName("Img2Txt");

    // Nettoyer les fichiers temporaires au démarrage
    FenetreChargement::nettoyerFichiersTemporaires();

    Img2txt_20 window;
    window.show();

    int result = app.exec();

    // Nettoyer les fichiers temporaires à la fermeture
    FenetreChargement::nettoyerFichiersTemporaires();

    return result;
}
