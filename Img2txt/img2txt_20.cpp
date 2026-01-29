#include "Img2txt_20.h"

Img2txt_20::Img2txt_20(QWidget* parent)
    : QMainWindow(parent)
    , fenetreCalibrage(nullptr)
{
    ui.setupUi(this);

    setWindowTitle("Img2Txt - Convertisseur d'images en ASCII Art");
    resize(800, 600);

    // Créer le QStackedWidget pour gerer les fenêtres
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    // Creer et ajouter la fenetre de chargement
    fenetreChargement = new FenetreChargement(this);
    stackedWidget->addWidget(fenetreChargement);

    // Connecter le signal de l'image chargee
    connect(fenetreChargement, &FenetreChargement::imagePGMChargee,
        this, &Img2txt_20::onImageChargee);

    // Afficher la fenetre de chargement
    stackedWidget->setCurrentWidget(fenetreChargement);
}

Img2txt_20::~Img2txt_20()
{
}

void Img2txt_20::onImageChargee(const ImagePGM& image, const QString& chemin)
{
    // Supprimer l'ancienne fenetre de calibrage si elle existe
    if (fenetreCalibrage != nullptr) {
        stackedWidget->removeWidget(fenetreCalibrage);
        delete fenetreCalibrage;
    }

    // Cr�er une nouvelle fen�tre de calibrage avec l'image charg�e
    fenetreCalibrage = new FenetreCalibrage(image, chemin, this);
    stackedWidget->addWidget(fenetreCalibrage);

    // Connecter le signal de retour
    connect(fenetreCalibrage, &FenetreCalibrage::retourChargement,
        this, &Img2txt_20::onRetourChargement);

    // Afficher la fen�tre de calibrage
    stackedWidget->setCurrentWidget(fenetreCalibrage);
}

void Img2txt_20::onRetourChargement()
{
    // Retourner � la fen�tre de chargement
    stackedWidget->setCurrentWidget(fenetreChargement);
}

