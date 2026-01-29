#include "FenetreCalibrage.h"
#include <QSplitter>
#include <QDir>
#include <QApplication>

FenetreCalibrage::FenetreCalibrage(const ImagePGM& image, const QString& chemin, QWidget* parent)
    : QWidget(parent)
    , imageOriginale(image)
    , imageActuelle(image)
    , cheminPGM(chemin)
    , majRatioEnCours(false)
{
    paletteActuelle = getPaletteParDefaut();

    // Calculer le ratio original
    ratioOriginal = static_cast<double>(image.largeur) / static_cast<double>(image.hauteur);

    setupUI();
    actualiserApercu();
}

FenetreCalibrage::~FenetreCalibrage()
{
}

void FenetreCalibrage::setupUI()
{
    setWindowTitle(QString::fromUtf8("Img2Txt 3.0 - Convertisseur d'images en ASCII Art"));
    resize(1200, 700);

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setSpacing(0);
    layoutPrincipal->setContentsMargins(0, 0, 0, 0);

    // === TITRE ===
    QLabel* titre = new QLabel(QString::fromUtf8("Aperçu de l'ASCII Art"));
    titre->setAlignment(Qt::AlignCenter);
    titre->setStyleSheet("font-size: 18px; font-weight: bold; padding: 15px; background-color: #2B2B2B; color: white;");
    layoutPrincipal->addWidget(titre);

    // === ZONE CENTRALE: Layout horizontal avec splitter ===
    QHBoxLayout* layoutCentral = new QHBoxLayout();
    layoutCentral->setSpacing(0);
    layoutCentral->setContentsMargins(0, 0, 0, 0);

    // --- PANNEAU GAUCHE ---
    QWidget* panneauGauche = new QWidget();
    panneauGauche->setMinimumWidth(350);
    panneauGauche->setMaximumWidth(400);
    panneauGauche->setStyleSheet("background-color: #3C3F41;");
    QVBoxLayout* layoutPanneau = new QVBoxLayout(panneauGauche);

    // Header du panneau
    QLabel* titrePanneau = new QLabel(QString::fromUtf8("Calibrage de l'image"));
    titrePanneau->setStyleSheet("font-weight: bold; font-size: 16px; color: white; padding: 10px;");
    titrePanneau->setAlignment(Qt::AlignCenter);
    layoutPanneau->addWidget(titrePanneau);

    // Info image
    lblInfo = new QLabel();
    lblInfo->setStyleSheet("padding: 10px; background-color: #2B2B2B; color: white; border-radius: 5px; margin: 5px;");
    lblInfo->setWordWrap(true);
    QString info = QString::fromUtf8("<b>Image chargée :</b><br>"
        "Dimensions originales : %1 x %2<br>"
        "Fichier : %3")
        .arg(imageOriginale.largeur)
        .arg(imageOriginale.hauteur)
        .arg(QFileInfo(cheminPGM).fileName());
    lblInfo->setText(info);
    layoutPanneau->addWidget(lblInfo);

    // Groupe Palette
    QLabel* lblPalette = new QLabel(QString::fromUtf8("Palettes"));
    lblPalette->setStyleSheet("font-weight: bold; color: white; padding: 10px 10px 5px 10px;");
    layoutPanneau->addWidget(lblPalette);

    comboPalette = new QComboBox();
    comboPalette->setStyleSheet("QComboBox { background-color: #2B2B2B; color: white; padding: 5px; border: 1px solid #4A90E2; } QComboBox:hover { border: 1px solid #357ABD; }");
    comboPalette->addItem(QString::fromUtf8("Par défaut"));
    comboPalette->addItem(QString::fromUtf8("Classique Étendue"));
    comboPalette->addItem("Blocs");
    comboPalette->addItem(QString::fromUtf8("Nature/Lumière"));
    comboPalette->addItem(QString::fromUtf8("Détails fins"));
    comboPalette->addItem(QString::fromUtf8("Ombre/Lumière"));
    comboPalette->addItem("Cyberpunk");
    comboPalette->addItem(QString::fromUtf8("Médiéval"));
    comboPalette->addItem("Aquarelle");
    comboPalette->addItem(QString::fromUtf8("Haute Définition"));
    comboPalette->addItem("Gradients");
    comboPalette->addItem("2-Bit (Noir & Blanc)");
    connect(comboPalette, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &FenetreCalibrage::onChangerPalette);
    connect(comboPalette, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &FenetreCalibrage::actualiserApercu);
    layoutPanneau->addWidget(comboPalette);

    // Groupe Redimensionnement
    QLabel* lblRedim = new QLabel(QString::fromUtf8("Redimensionnement"));
    lblRedim->setStyleSheet("font-weight: bold; color: white; padding: 10px 10px 5px 10px;");
    layoutPanneau->addWidget(lblRedim);

    QHBoxLayout* layoutLargeur = new QHBoxLayout();
    QLabel* lblLarg = new QLabel(QString::fromUtf8("Largeur :"));
    lblLarg->setStyleSheet("color: white; padding-left: 10px;");
    layoutLargeur->addWidget(lblLarg);
    spinLargeur = new QSpinBox();
    spinLargeur->setStyleSheet("QSpinBox { background-color: #2B2B2B; color: white; padding: 5px; border: 1px solid #4A90E2; }");
    spinLargeur->setMinimum(10);
    spinLargeur->setMaximum(5000);
    spinLargeur->setValue(imageOriginale.largeur);
    connect(spinLargeur, QOverload<int>::of(&QSpinBox::valueChanged),
        this, &FenetreCalibrage::onLargeurChanged);
    layoutLargeur->addWidget(spinLargeur);
    layoutPanneau->addLayout(layoutLargeur);

    QHBoxLayout* layoutHauteur = new QHBoxLayout();
    QLabel* lblHaut = new QLabel(QString::fromUtf8("Hauteur :"));
    lblHaut->setStyleSheet("color: white; padding-left: 10px;");
    layoutHauteur->addWidget(lblHaut);
    spinHauteur = new QSpinBox();
    spinHauteur->setStyleSheet("QSpinBox { background-color: #2B2B2B; color: white; padding: 5px; border: 1px solid #4A90E2; }");
    spinHauteur->setMinimum(10);
    spinHauteur->setMaximum(5000);
    spinHauteur->setValue(imageOriginale.hauteur);
    connect(spinHauteur, QOverload<int>::of(&QSpinBox::valueChanged),
        this, &FenetreCalibrage::onHauteurChanged);
    layoutHauteur->addWidget(spinHauteur);
    layoutPanneau->addLayout(layoutHauteur);

    // Checkbox Inverser
    QLabel* lblEffets = new QLabel(QString::fromUtf8("Inverser les couleurs"));
    lblEffets->setStyleSheet("font-weight: bold; color: white; padding: 10px 10px 5px 10px;");
    layoutPanneau->addWidget(lblEffets);

    checkInverser = new QCheckBox(QString::fromUtf8("Inverser les couleurs"));
    checkInverser->setStyleSheet("QCheckBox { color: white; padding-left: 10px; }");
    connect(checkInverser, &QCheckBox::stateChanged,
        this, &FenetreCalibrage::actualiserApercu);
    layoutPanneau->addWidget(checkInverser);

    layoutPanneau->addStretch();

    // Statut
    lblStatut = new QLabel("");
    lblStatut->setStyleSheet("color: #27AE60; font-weight: bold; padding: 10px; background-color: #2B2B2B;");
    lblStatut->setAlignment(Qt::AlignCenter);
    lblStatut->setWordWrap(true);
    layoutPanneau->addWidget(lblStatut);

    // Boutons en bas
    btnEnregistrer = new QPushButton();
    btnEnregistrer->setIcon(QIcon(":/illustrations/download.png"));
    btnEnregistrer->setIconSize(QSize(32, 32));
    btnEnregistrer->setText(QString::fromUtf8("  Enregistrer"));
    btnEnregistrer->setMinimumHeight(50);
    btnEnregistrer->setStyleSheet("QPushButton { background-color: #27AE60; color: white; font-weight: bold; border-radius: 5px; text-align: left; padding-left: 10px; } QPushButton:hover { background-color: #229954; }");
    connect(btnEnregistrer, &QPushButton::clicked, this, &FenetreCalibrage::onEnregistrer);
    layoutPanneau->addWidget(btnEnregistrer);

    btnRetour = new QPushButton(QString::fromUtf8("< Retour"));
    btnRetour->setMinimumHeight(45);
    btnRetour->setStyleSheet("QPushButton { background-color: #7F8C8D; color: white; font-weight: bold; border-radius: 5px; } QPushButton:hover { background-color: #95A5A6; }");
    connect(btnRetour, &QPushButton::clicked, this, &FenetreCalibrage::onRetour);
    layoutPanneau->addWidget(btnRetour);

    layoutCentral->addWidget(panneauGauche);

    // --- PANNEAU DROIT: Aperçu ---
    QWidget* panneauApercu = new QWidget();
    panneauApercu->setStyleSheet("background-color: #2B2B2B;");
    QVBoxLayout* layoutApercu = new QVBoxLayout(panneauApercu);
    layoutApercu->setSpacing(5);
    layoutApercu->setContentsMargins(5, 5, 5, 5);

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { background-color: #2B2B2B; border: none; }");

    textApercu = new QTextEdit();
    textApercu->setReadOnly(true);
    textApercu->setFont(QFont("Courier", 8));
    textApercu->setStyleSheet("background-color: black; color: white; border: 1px solid #4A90E2;");

    scrollArea->setWidget(textApercu);
    layoutApercu->addWidget(scrollArea);

    layoutCentral->addWidget(panneauApercu, 1);

    layoutPrincipal->addLayout(layoutCentral, 1);
}

void FenetreCalibrage::onChangerPalette(int index)
{
    switch (index) {
    case 0: paletteActuelle = getPaletteParDefaut(); break;
    case 1: paletteActuelle = getPaletteClassiqueEtendue(); break;
    case 2: paletteActuelle = getPaletteBlocs(); break;
    case 3: paletteActuelle = getPaletteNatureLumiere(); break;
    case 4: paletteActuelle = getPaletteDetailsFins(); break;
    case 5: paletteActuelle = getPaletteOmbreLumiere(); break;
    case 6: paletteActuelle = getPaletteCyberpunk(); break;
    case 7: paletteActuelle = getPaletteMedieval(); break;
    case 8: paletteActuelle = getPaletteAquarelle(); break;
    case 9: paletteActuelle = getPaletteHauteDefinition(); break;
    case 10: paletteActuelle = getPaletteGradients(); break;
    case 11: paletteActuelle = getPalette2Bit(); break;
    default: paletteActuelle = getPaletteParDefaut(); break;
    }
}

void FenetreCalibrage::onRedimensionner()
{
    int nouvelleLargeur = spinLargeur->value();
    int nouvelleHauteur = spinHauteur->value();

    if (nouvelleLargeur != imageOriginale.largeur || nouvelleHauteur != imageOriginale.hauteur) {
        imageActuelle = redimensionnerImage(imageOriginale, nouvelleLargeur, nouvelleHauteur);

        actualiserApercu();

        lblStatut->setText(QString::fromUtf8("✓ Image redimensionnée"));
        lblStatut->setStyleSheet("color: #27AE60; font-weight: bold; padding: 10px; background-color: #2B2B2B;");
    }
}

// ✨ Maintien du ratio lors du changement de largeur
void FenetreCalibrage::onLargeurChanged(int valeur)
{
    if (majRatioEnCours) return;  // Éviter les boucles infinies

    majRatioEnCours = true;

    // Calculer la nouvelle hauteur en maintenant le ratio
    int nouvelleHauteur = static_cast<int>(valeur / ratioOriginal);
    if (nouvelleHauteur < spinHauteur->minimum()) nouvelleHauteur = spinHauteur->minimum();
    if (nouvelleHauteur > spinHauteur->maximum()) nouvelleHauteur = spinHauteur->maximum();

    spinHauteur->setValue(nouvelleHauteur);

    majRatioEnCours = false;

    onRedimensionner();
}

// ✨ Maintien du ratio lors du changement de hauteur
void FenetreCalibrage::onHauteurChanged(int valeur)
{
    if (majRatioEnCours) return;  // Éviter les boucles infinies

    majRatioEnCours = true;

    // Calculer la nouvelle largeur en maintenant le ratio
    int nouvelleLargeur = static_cast<int>(valeur * ratioOriginal);
    if (nouvelleLargeur < spinLargeur->minimum()) nouvelleLargeur = spinLargeur->minimum();
    if (nouvelleLargeur > spinLargeur->maximum()) nouvelleLargeur = spinLargeur->maximum();

    spinLargeur->setValue(nouvelleLargeur);

    majRatioEnCours = false;

    onRedimensionner();
}

void FenetreCalibrage::onRecharger()
{
    lblStatut->setText(QString::fromUtf8("⏳ Génération en cours..."));
    lblStatut->setStyleSheet("color: #FFA500; font-weight: bold; padding: 10px; background-color: #2B2B2B;");
    QApplication::processEvents();

    actualiserApercu();

    lblStatut->setText(QString::fromUtf8("✓ Aperçu rechargé"));
    lblStatut->setStyleSheet("color: #27AE60; font-weight: bold; padding: 10px; background-color: #2B2B2B;");
}

void FenetreCalibrage::actualiserApercu()
{
    ImagePGM imageAffichee = imageActuelle;

    if (checkInverser->isChecked()) {
        imageAffichee = inverserCouleurs(imageAffichee);
    }

    QString apercu = QString::fromStdString(genererAsciiArt(imageAffichee, paletteActuelle));
    textApercu->setText(apercu);
}

void FenetreCalibrage::onEnregistrer()
{
    QString dossierRendus = QDir::currentPath() + "/rendus";
    QDir().mkpath(dossierRendus);

    QString nomBase = QFileInfo(cheminPGM).baseName();
    QString nomFichier = QString("%1/%2_ascii.txt").arg(dossierRendus).arg(nomBase);

    nomFichier = QFileDialog::getSaveFileName(this,
        QString::fromUtf8("Enregistrer l'ASCII Art"),
        nomFichier,
        "Fichiers texte (*.txt);;Tous les fichiers (*)");

    if (nomFichier.isEmpty()) {
        return;
    }

    try {
        ImagePGM imageFinal = imageActuelle;

        if (checkInverser->isChecked()) {
            imageFinal = inverserCouleurs(imageFinal);
        }

        sauvegarderAsciiArt(imageFinal, paletteActuelle, nomFichier.toStdString());

        QMessageBox::information(this, QString::fromUtf8("Succes"),
            QString::fromUtf8("ASCII Art enregistre avec succes :\n%1").arg(nomFichier));

        lblStatut->setText(QString::fromUtf8("Fichier enregistre : ") + QFileInfo(nomFichier).fileName());
        lblStatut->setStyleSheet("color: green; font-weight: bold;");

    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Erreur",
            QString("Erreur lors de l'enregistrement : %1").arg(e.what()));
    }
}

void FenetreCalibrage::onInverser()
{
    imageActuelle = inverserCouleurs(imageActuelle);
    actualiserApercu();
}

void FenetreCalibrage::onRetour()
{
    emit retourChargement();
}
