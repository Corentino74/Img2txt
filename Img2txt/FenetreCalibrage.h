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
#include <QRadioButton>
#include <QButtonGroup>
#include <QStackedWidget>
#include <QLineEdit>
#include "fonctions.h"

// Modes de calibrage
enum class ModeCalibrage {
    SIMPLE,          // Mode monochrome classique
    COULEUR_1CARA,   // 1 caractère avec couleurs
    COULEUR_MULTI    // Palette ASCII + couleurs
};

class FenetreCalibrage : public QWidget
{
    Q_OBJECT

public:
    explicit FenetreCalibrage(const ImagePGM& image, const QString& cheminPGM, const QString& cheminOriginal, QWidget* parent = nullptr);
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
    void onModeChange();                  // Changement de mode (Simple/Couleur1/Multi)

private:
    void setupUI();
    void actualiserApercu();
    QString genererApercu();
    void animerPanneau(bool visible);     // Animation du panneau

    ImagePGM imageOriginale;
    ImagePGM imageActuelle;
    QString cheminPGM;
    std::vector<std::string> paletteActuelle;

    // État couleur (lazy loading)
    ModeCalibrage modeActuel;
    Image imageCouleur;                   // Chargée uniquement si mode couleur
    Image imageCouleurOriginale;          // Version originale pour redimensionnement
    bool imageCouleurChargee;
    QString cheminImageOriginale;
    PaletteCouleur paletteCouleurActuelle;

    // Maintien du ratio
    double ratioOriginal;                 // Ratio largeur/hauteur
    bool majRatioEnCours;                 // Éviter les boucles infinies

    // Sélection de mode
    QRadioButton* radioSimple;
    QRadioButton* radioCouleur1;
    QRadioButton* radioCouleurMulti;
    QButtonGroup* groupMode;

    // Panel dynamique (3 modes)
    QStackedWidget* stackedPanels;

    // Widgets communs
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

    // Widgets mode Couleur 1 cara
    QComboBox* comboCaractere;
    QLineEdit* lineCaractereCustom;
    QComboBox* comboPaletteCouleur1;
    QCheckBox* checkInverserCouleur1;
    QLabel* lblWarningCouleur1;

    // Widgets mode Couleur Multi
    QComboBox* comboPaletteAscii;
    QComboBox* comboPaletteCouleurMulti;
    QSpinBox* spinRed;
    QSpinBox* spinGreen;
    QSpinBox* spinBlue;
    QCheckBox* checkInverserCouleurMulti;
    QLabel* lblWarningCouleurMulti;
};
