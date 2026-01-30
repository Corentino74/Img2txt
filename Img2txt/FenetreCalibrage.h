/*
FenetreCalibrage.h
Deuxieme fenêtre pour le calibrage :
- 11 palettes prédéfinies
- Redimensionnement d'image (10-5000 pixels)
- Inversion des couleurs
- Aperçu en temps réel
- Bouton "Recharger" pour mettre à jour l'aperçu
- Sauvegarde dans le dossier `rendus/`
- Bouton "Retour" vers la fenêtre de chargement
*/

#pragma once

#include <QWidget>
#include <QString>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollArea>
#include <QPropertyAnimation>
#include "fonctions.h"

class FenetreCalibrage : public QWidget
{
    Q_OBJECT

public:
    explicit FenetreCalibrage(const ImagePGM& image, const QString& chemin, QWidget* parent = nullptr);
    ~FenetreCalibrage();

signals:
    void retourChargement();

private slots:
    void onRecharger();
    void onEnregistrer();
    void onInverser();
    void onRedimensionner();
    void onRetour();
    void onChangerPalette(int index);
    void onLargeurChanged(int valeur);    // Maintien du ratio
    void onHauteurChanged(int valeur);    // Maintien du ratio

private:
    void setupUI();
    void actualiserApercu();
    QString genererApercu();
    void animerPanneau(bool visible);     // Animation du panneau

    ImagePGM imageOriginale;
    ImagePGM imageActuelle;
    QString cheminPGM;
    std::vector<std::string> paletteActuelle;

    // Maintien du ratio
    double ratioOriginal;                 // Ratio largeur/hauteur
    bool majRatioEnCours;                 // Éviter les boucles infinies

    QComboBox* comboPalette;
    QSpinBox* spinLargeur;
    QSpinBox* spinHauteur;
    QCheckBox* checkInverser;
    QPushButton* btnRecharger;
    QPushButton* btnEnregistrer;
    QPushButton* btnRetour;
    QTextEdit* textApercu;
    QLabel* lblInfo;
    QLabel* lblStatut;
};
