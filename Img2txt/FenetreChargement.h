/*
FenetreChargement.h
Premi�re fen�tre pour la gestion des fichiers :
- Liste automatique des images PNG/JPG du dossier `images/`
- Double-clic pour convertir automatiquement
- Chargement manuel de fichiers PGM
- Conversion manuelle PNG/JPG vers PGM
- Bouton "Suivant" activ� uniquement avec une image valide
*/

#pragma once

#include <QWidget>
#include <QListWidget>
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
    void imagePGMChargee(const ImagePGM& image, const QString& chemin);

private slots:
    void onChargerPGM();              // Charger un fichier (PNG/JPG/PGM)
    void onImageListeClicked(QListWidgetItem* item);
    void onConvertirManuel();
    void onAide();                    // Afficher l'aide
    void onSupprimer();               // Supprimer l'image chargée
    void onConvertir();               // Convertir => ASCII

private:
    void setupUI();
    void chargerListeImages();
    void afficherInfoImage();
    void afficherPreview();           // Afficher preview de l'image
    void resetPreview();              // Réinitialiser la preview
    void convertirEtChargerImage(const QString& cheminImage);
    QString creerDossierTemp();       // Créer dossier temporaire pour PGM

    // Support du Drag & Drop
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

    // Widgets - Liste
    QListWidget* listeImages;

    // Widgets - Preview/Drop zone
    QLabel* lblPreview;               // Zone de preview/drop
    bool imagePreviewActive;          // true si une image est chargée

    // Widgets - Boutons avec icônes
    QPushButton* btnAide;             // "?" Aide
    QPushButton* btnCharger;          // "Charger" (remplace btnChargerPGM + btnConvertirManuel)
    QPushButton* btnSupprimer;        // "Supprimer" l'image
    QPushButton* btnConvertir;        // "Convertir" => (remplace btnSuivant)

    QLabel* lblInfo;
    QLabel* lblStatut;

    bool imageValide;                 // Déclaré avant imageChargee pour ordre d'initialisation
    ImagePGM imageChargee;
    QString cheminPGM;
    QString cheminImageOriginale;     // PNG/JPG original pour preview
    QString dossierImages;
    QString dossierRendus;
    QString dossierTemp;              // Dossier temporaire pour PGM
};
