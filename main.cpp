// force et honneur pour ceux qui veulent reprendre ce code, j'ai fait de mon mieux :(
// questions ? corentinchitwood@gmail.com
#include "Img2txt/img2txt_20.h"
#include "Img2txt/FenetreChargement.h"
#include <QtWidgets/QApplication>
#include <QIcon>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // Configuration des params de l'app
    app.setApplicationName("Img2txt");
    app.setApplicationVersion("4.0");
    app.setOrganizationName("Corentin");
    app.setOrganizationDomain("img2txt.app");
    // Icône de l'app pour Windows (linux et macos -> qrc)
    app.setWindowIcon(QIcon(":/illustrations/app_icon.png"));

    // execution principale
    FenetreChargement::nettoyerFichiersTemporaires();
    Img2txt_20 window;
    window.show();
    int result = app.exec();    // Lancement de l'application
    FenetreChargement::nettoyerFichiersTemporaires();

    return result;
}