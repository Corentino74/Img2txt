/*
FenetreChargement.h
Premi�re fen�tre pour la gestion des fichiers :
- Chargement par explorateur de fichiers (bouton)
- Chargement par drag & drop
- Conversion vers FenetreCalibrage
*/

#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QPixmap>
#include "fonctions.h"

class FenetreChargement : public QWidget
{
    Q_OBJECT

public:
    explicit FenetreChargement(QWidget* parent = nullptr);
    ~FenetreChargement();

    ImagePGM getImageChargee() const { return imageChargee; }
    QString getCheminPGM() const { return cheminPGM; }
    bool isImageValide() const { return imageValide; }

    // Nettoyage des fichiers temporaires
    static void nettoyerFichiersTemporaires();

signals:
    void imagePGMChargee(const ImagePGM& image, const QString& cheminPGM, const QString& cheminOriginal);
    void calibrageCouleurDemande(const QString& cheminImage);

private slots:
    void onChargerPGM();              // Charger un fichier (PNG/JPG/PGM)
    void onSupprimer();               // Supprimer l'image chargée
    void onConvertir();               // Convertir => Ouvre FenetreCalibrage

private:
    void setupUI();
    void afficherInfoImage();
    void afficherPreview();           // Afficher preview de l'image
    void resetPreview();              // Réinitialiser la preview
    void convertirEtChargerImage(const QString& cheminImage);
    QString creerDossierTemp();       // Créer dossier temporaire pour PGM

    // Support du Drag & Drop
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

    // Widgets
    QLabel* lblPreview;               // Zone de preview/drop
    QPushButton* btnCharger;          
    QPushButton* btnSupprimer;        
    QPushButton* btnConvertir;        
    QLabel* lblInfo;
    QLabel* lblStatut;

    // État
    bool imageValide;                 
    ImagePGM imageChargee;
    QString cheminPGM;
    QString cheminImageOriginale;     
    QString dossierTemp;              // Dossier temporaire pour PGM
};
