#include "img2txt_20.h"

// Implémentation de la classe principale de l'application
// - Gère la navigation entre les différentes fenêtres
Img2txt_20::Img2txt_20(QWidget* parent) : QMainWindow(parent)
    , fenetreCalibrage(nullptr)
{
    ui.setupUi(this); 

    setWindowTitle("Img2txt - Convertisseur d'images en ASCII Art");
    resize(800, 700);

    stackedWidget = new QStackedWidget(this); // Widget pour empiler les différentes fenêtres
    setCentralWidget(stackedWidget);

    fenetreChargement = new FenetreChargement(this);
    stackedWidget->addWidget(fenetreChargement);

    connect(fenetreChargement, &FenetreChargement::imagePGMChargee,
        this, &Img2txt_20::onImageChargee);

    stackedWidget->setCurrentWidget(fenetreChargement);
}

// destructeur, inutile 
Img2txt_20::~Img2txt_20() {}

// Slot pour recevoir l'image chargée depuis FenetreChargement
void Img2txt_20::onImageChargee(const ImagePGM& image, const QString& cheminPGM, const QString& cheminOriginal)
{
    // Si une fenêtre de calibrage existe déjà, la supprimer avant d'en créer une nouvelle
    if (fenetreCalibrage != nullptr) {
        stackedWidget->removeWidget(fenetreCalibrage);
        delete fenetreCalibrage;
    }

    fenetreCalibrage = new FenetreCalibrage(image, cheminPGM, cheminOriginal, this);
    stackedWidget->addWidget(fenetreCalibrage);

    connect(fenetreCalibrage, &FenetreCalibrage::retourChargement,
        this, &Img2txt_20::onRetourChargement);

    stackedWidget->setCurrentWidget(fenetreCalibrage);
}

void Img2txt_20::onRetourChargement()
{
    stackedWidget->setCurrentWidget(fenetreChargement);
}