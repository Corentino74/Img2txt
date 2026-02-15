#include "FenetreCalibrage.h"
#include <QSplitter>
#include <QDir>
#include <QApplication>
#include <QTextDocument>
#include <sstream>

FenetreCalibrage::FenetreCalibrage(const ImagePGM& image, const QString& cheminPGM, const QString& cheminOriginal, QWidget* parent)
    : QWidget(parent)
    , imageOriginale(image)
    , imageActuelle(image)
    , cheminPGM(cheminPGM)
    , modeActuel(ModeCalibrage::SIMPLE)
    , imageCouleurChargee(false)
    , majRatioEnCours(false)
{
    paletteActuelle = getPaletteParDefaut();

    // Stocker le chemin ORIGINAL pour lazy loading couleur
    cheminImageOriginale = cheminOriginal;

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
#include <QWidget>
#include <QString>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
    setWindowTitle(QString::fromUtf8("Img2txt v4.0 - Convertisseur d'images en ASCII Art"));
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

    // === SÉLECTION DU MODE ===
    QLabel* lblMode = new QLabel(QString::fromUtf8("Mode de conversion"));
    lblMode->setStyleSheet("font-weight: bold; color: white; padding: 10px 10px 5px 10px;");
    layoutPanneau->addWidget(lblMode);

    QWidget* widgetModes = new QWidget();
    widgetModes->setStyleSheet("background-color: #2B2B2B; border-radius: 5px; margin: 5px;");
    QVBoxLayout* layoutModes = new QVBoxLayout(widgetModes);
    layoutModes->setSpacing(5);

    groupMode = new QButtonGroup(this);

    radioSimple = new QRadioButton(QString::fromUtf8("Simple (monochrome)"));
    radioSimple->setStyleSheet("color: white; padding: 5px;");
    radioSimple->setChecked(true);
    groupMode->addButton(radioSimple, static_cast<int>(ModeCalibrage::SIMPLE));
    layoutModes->addWidget(radioSimple);

    radioCouleur1 = new QRadioButton(QString::fromUtf8("Couleur 1 cara"));
    radioCouleur1->setStyleSheet("color: white; padding: 5px;");
    groupMode->addButton(radioCouleur1, static_cast<int>(ModeCalibrage::COULEUR_1CARA));
    layoutModes->addWidget(radioCouleur1);

    radioCouleurMulti = new QRadioButton(QString::fromUtf8("Couleur multi"));
    radioCouleurMulti->setStyleSheet("color: white; padding: 5px;");
    groupMode->addButton(radioCouleurMulti, static_cast<int>(ModeCalibrage::COULEUR_MULTI));
    layoutModes->addWidget(radioCouleurMulti);

    connect(groupMode, &QButtonGroup::buttonClicked,
            this, &FenetreCalibrage::onModeChange);

    layoutPanneau->addWidget(widgetModes);

    // === PANELS DYNAMIQUES PAR MODE ===
    stackedPanels = new QStackedWidget();
    stackedPanels->setStyleSheet("background-color: transparent;");

    // --- PANEL 1: MODE SIMPLE ---
    QWidget* panelSimple = new QWidget();
    QVBoxLayout* layoutSimple = new QVBoxLayout(panelSimple);
    layoutSimple->setContentsMargins(0, 0, 0, 0);
    layoutSimple->setSpacing(5);

    // Groupe Palette
    QLabel* lblPalette = new QLabel(QString::fromUtf8("Palettes"));
    lblPalette->setStyleSheet("font-weight: bold; color: white; padding: 2px 10px 0px 10px;");
    layoutSimple->addWidget(lblPalette);

    comboPalette = new QComboBox();
    comboPalette->setStyleSheet("QComboBox { background-color: #2B2B2B; color: white; padding: 5px; border: 1px solid #4A90E2; } QComboBox:hover { border: 1px solid #357ABD; }");
    comboPalette->addItem("Normale");
    comboPalette->addItem("Classique");
    comboPalette->addItem(QString::fromUtf8("Classique Étendue"));
    comboPalette->addItem("Blocs");
    comboPalette->addItem("Clair Obscur");
    comboPalette->addItem(QString::fromUtf8("Nature/Lumière"));
    comboPalette->addItem(QString::fromUtf8("Médiéval"));
    comboPalette->addItem(QString::fromUtf8("Détails fins"));
    comboPalette->addItem("Aquarelle");
    comboPalette->addItem("Gradients");
    comboPalette->addItem("Cyberpunk");
    comboPalette->addItem("2-Bit (Noir & Blanc)");
    comboPalette->addItem(QString::fromUtf8("Haute Définition"));
    comboPalette->addItem("Lettres Seules");
    comboPalette->addItem(QString::fromUtf8("Détourage"));
    comboPalette->addItem("Reflet");
    comboPalette->addItem("Points (Dithering)");
    comboPalette->addItem(QString::fromUtf8("Lignes (Rétro)"));
    comboPalette->addItem("Stippling");
    comboPalette->addItem("Tramage");
    comboPalette->addItem("Minimaliste");
    comboPalette->addItem("Monospace");
    connect(comboPalette, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &FenetreCalibrage::onChangerPalette);
    connect(comboPalette, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &FenetreCalibrage::actualiserApercu);
    layoutSimple->addWidget(comboPalette);

    // Checkbox Inverser (mode simple uniquement)
    checkInverser = new QCheckBox(QString::fromUtf8("Inverser les couleurs"));
    checkInverser->setStyleSheet("QCheckBox { color: white; padding: 5px; }");
    checkInverser->setChecked(true);
    connect(checkInverser, &QCheckBox::stateChanged,
        this, &FenetreCalibrage::actualiserApercu);
    layoutSimple->addWidget(checkInverser);

    stackedPanels->addWidget(panelSimple);

    // --- PANEL 2: MODE COULEUR 1 CARA ---
    QWidget* panelCouleur1 = new QWidget();
    QVBoxLayout* layoutCouleur1 = new QVBoxLayout(panelCouleur1);
    layoutCouleur1->setContentsMargins(0, 0, 0, 0);
    layoutCouleur1->setSpacing(5);

    QLabel* lblCaractere = new QLabel(QString::fromUtf8("Caractère à utiliser"));
    lblCaractere->setStyleSheet("font-weight: bold; color: white; padding: 10px 10px 5px 10px;");
    layoutCouleur1->addWidget(lblCaractere);

    comboCaractere = new QComboBox();
    comboCaractere->setStyleSheet("QComboBox { background-color: #2B2B2B; color: white; padding: 5px; border: 1px solid #4A90E2; }");
    comboCaractere->addItem(QString::fromUtf8("█ - Bloc plein"));
    comboCaractere->addItem(QString::fromUtf8("▓ - Bloc moyen"));
    comboCaractere->addItem(QString::fromUtf8("▒ - Bloc léger"));
    comboCaractere->addItem(QString::fromUtf8("░ - Bloc très léger"));
    comboCaractere->addItem(QString::fromUtf8("@ - Arobase"));
    comboCaractere->addItem(QString::fromUtf8("# - Dièse"));
    comboCaractere->addItem(QString::fromUtf8("* - Étoile"));
    comboCaractere->addItem(QString::fromUtf8("+ - Plus"));
    connect(comboCaractere, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &FenetreCalibrage::actualiserApercu);
    layoutCouleur1->addWidget(comboCaractere);

    QLabel* lblCustom = new QLabel(QString::fromUtf8("ou Caractère personnalisé :"));
    lblCustom->setStyleSheet("color: white; padding: 10px 10px 5px 10px;");
    layoutCouleur1->addWidget(lblCustom);

    lineCaractereCustom = new QLineEdit();
    lineCaractereCustom->setStyleSheet("QLineEdit { background-color: #2B2B2B; color: white; padding: 5px; border: 1px solid #4A90E2; }");
    lineCaractereCustom->setPlaceholderText(QString::fromUtf8("Tapez un caractère ou mot..."));
    connect(lineCaractereCustom, &QLineEdit::textChanged,
        this, &FenetreCalibrage::actualiserApercu);
    layoutCouleur1->addWidget(lineCaractereCustom);

    comboPaletteCouleur1 = new QComboBox();
    comboPaletteCouleur1->setStyleSheet("QComboBox { background-color: #2B2B2B; color: white; padding: 5px; border: 1px solid #4A90E2; }");
    comboPaletteCouleur1->addItem("8 couleurs ANSI");
    comboPaletteCouleur1->addItem("16 couleurs ANSI");
    comboPaletteCouleur1->addItem("32 couleurs");
    comboPaletteCouleur1->addItem("64 couleurs (calcul intensif !)");
    comboPaletteCouleur1->addItem("128 couleurs (très coûteux !)");
    comboPaletteCouleur1->addItem("256 couleurs xterm (INSTABLE !)");
    comboPaletteCouleur1->setCurrentIndex(1); // 16 couleurs par défaut
    connect(comboPaletteCouleur1, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &FenetreCalibrage::actualiserApercu);
    layoutCouleur1->addWidget(comboPaletteCouleur1);

    // Checkbox Inverser couleurs pour mode couleur 1 cara
    checkInverserCouleur1 = new QCheckBox(QString::fromUtf8("Inverser les couleurs"));
    checkInverserCouleur1->setStyleSheet("QCheckBox { color: white; padding: 10px; background-color: #3B3B3B; }");
    checkInverserCouleur1->setChecked(false);
    connect(checkInverserCouleur1, &QCheckBox::stateChanged,
        this, &FenetreCalibrage::actualiserApercu);
    layoutCouleur1->addWidget(checkInverserCouleur1);

    lblWarningCouleur1 = new QLabel("");
    lblWarningCouleur1->setStyleSheet("color: #FFA500; padding: 5px; font-style: italic;");
    lblWarningCouleur1->setWordWrap(true);
    layoutCouleur1->addWidget(lblWarningCouleur1);

    stackedPanels->addWidget(panelCouleur1);

    // --- PANEL 3: MODE COULEUR MULTI ---
    QWidget* panelCouleurMulti = new QWidget();
    QVBoxLayout* layoutCouleurMulti = new QVBoxLayout(panelCouleurMulti);
    layoutCouleurMulti->setContentsMargins(0, 0, 0, 0);
    layoutCouleurMulti->setSpacing(5);

    QLabel* lblPaletteAscii = new QLabel(QString::fromUtf8("Palette ASCII"));
    lblPaletteAscii->setStyleSheet("font-weight: bold; color: white; padding: 10px 10px 5px 10px;");
    layoutCouleurMulti->addWidget(lblPaletteAscii);

    comboPaletteAscii = new QComboBox();
    comboPaletteAscii->setStyleSheet("QComboBox { background-color: #2B2B2B; color: white; padding: 5px; border: 1px solid #4A90E2; }");
    // Réutiliser TOUTES les palettes du mode simple
    comboPaletteAscii->addItem("Normale");
    comboPaletteAscii->addItem("Classique");
    comboPaletteAscii->addItem(QString::fromUtf8("Classique Étendue"));
    comboPaletteAscii->addItem("Blocs");
    comboPaletteAscii->addItem("Clair Obscur");
    comboPaletteAscii->addItem(QString::fromUtf8("Nature/Lumière"));
    comboPaletteAscii->addItem(QString::fromUtf8("Médiéval"));
    comboPaletteAscii->addItem(QString::fromUtf8("Détails fins"));
    comboPaletteAscii->addItem("Aquarelle");
    comboPaletteAscii->addItem("Gradients");
    comboPaletteAscii->addItem("Cyberpunk");
    comboPaletteAscii->addItem("2-Bit (Noir & Blanc)");
    comboPaletteAscii->addItem(QString::fromUtf8("Haute Définition"));
    comboPaletteAscii->addItem("Lettres Seules");
    comboPaletteAscii->addItem(QString::fromUtf8("Détourage"));
    comboPaletteAscii->addItem("Reflet");
    comboPaletteAscii->addItem("Points (Dithering)");
    comboPaletteAscii->addItem(QString::fromUtf8("Lignes (Rétro)"));
    comboPaletteAscii->addItem("Stippling");
    comboPaletteAscii->addItem("Tramage");
    comboPaletteAscii->addItem("Minimaliste");
    comboPaletteAscii->addItem("Monospace");
    connect(comboPaletteAscii, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &FenetreCalibrage::actualiserApercu);
    layoutCouleurMulti->addWidget(comboPaletteAscii);

    comboPaletteCouleurMulti = new QComboBox();
    comboPaletteCouleurMulti->setStyleSheet("QComboBox { background-color: #2B2B2B; color: white; padding: 5px; border: 1px solid #4A90E2; }");
    comboPaletteCouleurMulti->addItem("1 couleur (RGB personnalisé)");
    comboPaletteCouleurMulti->addItem("8 couleurs ANSI");
    comboPaletteCouleurMulti->addItem("16 couleurs ANSI");
    comboPaletteCouleurMulti->addItem("32 couleurs");
    comboPaletteCouleurMulti->addItem("64 couleurs (calcul intensif !)");
    comboPaletteCouleurMulti->addItem("128 couleurs (très coûteux !)");
    comboPaletteCouleurMulti->addItem("256 couleurs xterm (INSTABLE !)");
    comboPaletteCouleurMulti->setCurrentIndex(2); // 16 couleurs par défaut
    connect(comboPaletteCouleurMulti, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &FenetreCalibrage::actualiserApercu);
    layoutCouleurMulti->addWidget(comboPaletteCouleurMulti);

    // SpinBox RGB (visible si palette 1 couleur)
    QLabel* lblRGB = new QLabel(QString::fromUtf8("Couleur personnalisée (RGB) :"));
    lblRGB->setStyleSheet("color: white; padding: 10px 10px 5px 10px;");
    layoutCouleurMulti->addWidget(lblRGB);

    QHBoxLayout* layoutRGB = new QHBoxLayout();
    QLabel* lblR = new QLabel("R:");
    lblR->setStyleSheet("color: white;");
    layoutRGB->addWidget(lblR);
    spinRed = new QSpinBox();
    spinRed->setStyleSheet("QSpinBox { background-color: #2B2B2B; color: white; padding: 3px; }");
    spinRed->setRange(0, 255);
    spinRed->setValue(255);
    connect(spinRed, QOverload<int>::of(&QSpinBox::valueChanged),
        this, &FenetreCalibrage::actualiserApercu);
    layoutRGB->addWidget(spinRed);

    QLabel* lblG = new QLabel("G:");
    lblG->setStyleSheet("color: white;");
    layoutRGB->addWidget(lblG);
    spinGreen = new QSpinBox();
    spinGreen->setStyleSheet("QSpinBox { background-color: #2B2B2B; color: white; padding: 3px; }");
    spinGreen->setRange(0, 255);
    spinGreen->setValue(255);
    connect(spinGreen, QOverload<int>::of(&QSpinBox::valueChanged),
        this, &FenetreCalibrage::actualiserApercu);
    layoutRGB->addWidget(spinGreen);

    QLabel* lblB = new QLabel("B:");
    lblB->setStyleSheet("color: white;");
    layoutRGB->addWidget(lblB);
    spinBlue = new QSpinBox();
    spinBlue->setStyleSheet("QSpinBox { background-color: #2B2B2B; color: white; padding: 3px; }");
    spinBlue->setRange(0, 255);
    spinBlue->setValue(255);
    connect(spinBlue, QOverload<int>::of(&QSpinBox::valueChanged),
        this, &FenetreCalibrage::actualiserApercu);
    layoutRGB->addWidget(spinBlue);

    layoutCouleurMulti->addLayout(layoutRGB);

    // Checkbox Inverser couleurs pour mode couleur multi
    checkInverserCouleurMulti = new QCheckBox(QString::fromUtf8("Inverser les couleurs"));
    checkInverserCouleurMulti->setStyleSheet("QCheckBox { color: white; padding: 10px; background-color: #3B3B3B; }");
    checkInverserCouleurMulti->setChecked(false);
    connect(checkInverserCouleurMulti, &QCheckBox::stateChanged,
        this, &FenetreCalibrage::actualiserApercu);
    layoutCouleurMulti->addWidget(checkInverserCouleurMulti);

    lblWarningCouleurMulti = new QLabel("");
    lblWarningCouleurMulti->setStyleSheet("color: #FFA500; padding: 5px; font-style: italic;");
    lblWarningCouleurMulti->setWordWrap(true);
    layoutCouleurMulti->addWidget(lblWarningCouleurMulti);

    stackedPanels->addWidget(panelCouleurMulti);

    // Ajouter le stacked widget au panneau
    layoutPanneau->addWidget(stackedPanels);

    // === CONTRÔLES COMMUNS (redimensionnement) ===
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

// Changement de palette, à modifier pour en ajouter de nouvelles !!
void FenetreCalibrage::onChangerPalette(int index)
{
    switch (index) {
    case 0: paletteActuelle = getPaletteNormale(); break;
    case 1: paletteActuelle = getPaletteParDefaut(); break;
    case 2: paletteActuelle = getPaletteClassiqueEtendue(); break;
    case 3: paletteActuelle = getPaletteBlocs(); break;
    case 4: paletteActuelle = getPaletteOmbreLumiere(); break;
    case 5: paletteActuelle = getPaletteNatureLumiere(); break;
    case 6: paletteActuelle = getPaletteMedieval(); break;
    case 7: paletteActuelle = getPaletteDetailsFins(); break;
    case 8: paletteActuelle = getPaletteAquarelle(); break;
    case 9: paletteActuelle = getPaletteGradients(); break;
    case 10: paletteActuelle = getPaletteCyberpunk(); break;
    case 11: paletteActuelle = getPalette2Bit(); break;
    case 12: paletteActuelle = getPaletteSaturation(); break;
    case 13: paletteActuelle = getPaletteLettresSeules(); break;
    case 14: paletteActuelle = getPaletteDetourage(); break;
    case 15: paletteActuelle = getPaletteReflet(); break;
    case 16: paletteActuelle = getPalettePoints(); break;
    case 17: paletteActuelle = getPaletteLignes(); break;
    case 18: paletteActuelle = getPaletteStippling(); break;
    case 19: paletteActuelle = getPaletteDithering(); break;
    case 20: paletteActuelle = getPaletteMinimaliste(); break;
    case 21: paletteActuelle = getPaletteMonospace(); break;
    default: paletteActuelle = getPaletteParDefaut(); break;
    }
}

void FenetreCalibrage::onRedimensionner()
{
    int nouvelleLargeur = spinLargeur->value();
    int nouvelleHauteur = spinHauteur->value();

    qDebug() << "[onRedimensionner] DEBUT - Demande:" << nouvelleLargeur << "x" << nouvelleHauteur;
    
    // Toujours redimensionner depuis l'originale
    imageActuelle = redimensionnerImage(imageOriginale, nouvelleLargeur, nouvelleHauteur);
    qDebug() << "  imageActuelle après redim:" << imageActuelle.largeur << "x" << imageActuelle.hauteur;
    
    // Redimensionner l'image couleur DEPUIS l'originale stockée
    if (imageCouleurChargee) {
        qDebug() << "  imageCouleurOriginale:" << imageCouleurOriginale.largeur << "x" << imageCouleurOriginale.hauteur;
        qDebug() << "  imageCouleur AVANT redim:" << imageCouleur.largeur << "x" << imageCouleur.hauteur;
        
        imageCouleur = redimensionnerImageCouleur(imageCouleurOriginale, nouvelleLargeur, nouvelleHauteur);
        
        qDebug() << "  imageCouleur APRES redim:" << imageCouleur.largeur << "x" << imageCouleur.hauteur;
        qDebug() << "  pixelsRGB.size():" << imageCouleur.pixelsRGB.size() << "(attendu:" << (nouvelleLargeur*nouvelleHauteur) << ")";
    }
    
    qDebug() << "[onRedimensionner] FIN";
    actualiserApercu();
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

// Maintien du ratio lors du changement de hauteur
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
    QString apercu;

    switch (modeActuel) {
        case ModeCalibrage::SIMPLE: {
            // Mode classique monochrome
            ImagePGM imageAffichee = imageActuelle;
            if (checkInverser->isChecked()) {
                imageAffichee = inverserCouleurs(imageAffichee);
            }
            apercu = QString::fromStdString(genererAsciiArt(imageAffichee, paletteActuelle));
            break;
        }

        case ModeCalibrage::COULEUR_1CARA: {
            // Mode couleur mono-caractère
            if (!imageCouleurChargee) {
                apercu = QString::fromUtf8("Chargement de l'image couleur...");
                break;
            }

            // Récupérer le caractère
            QString caractere;
            if (!lineCaractereCustom->text().isEmpty()) {
                // Caractère personnalisé prioritaire
                caractere = lineCaractereCustom->text();
            } else {
                // Caractère du combo (extraire le premier char)
                QString comboText = comboCaractere->currentText();
                if (comboText.length() > 0) {
                    caractere = QString(comboText[0]);
                } else {
                    caractere = "#";
                }
            }

            // Récupérer la palette couleur
            int paletteIndex = comboPaletteCouleur1->currentIndex();
            PaletteCouleur paletteCoul;
            
            switch (paletteIndex) {
                case 0: // 8 couleurs
                    paletteCoul = getPaletteCouleur8();
                    break;
                case 1: // 16 couleurs
                    paletteCoul = getPaletteCouleur16();
                    break;
                case 2: // 32 couleurs
                    paletteCoul = getPaletteCouleur32();
                    break;
                case 3: // 64 couleurs
                    paletteCoul = getPaletteCouleur64();
                    lblWarningCouleur1->setText(QString::fromUtf8("⚠ Calcul intensif en cours..."));
                    break;
                case 4: // 128 couleurs
                    paletteCoul = getPaletteCouleur128();
                    lblWarningCouleur1->setText(QString::fromUtf8("⚠ Très coûteux ! Soyez patient..."));
                    break;
                case 5: // 256 couleurs
                    paletteCoul = getPaletteCouleur256();
                    lblWarningCouleur1->setText(QString::fromUtf8("⚠ INSTABLE ! Peut crasher..."));
                    break;
                default:
                    paletteCoul = getPaletteCouleur16();
            }

            // Générer la preview HTML avec couleurs
            std::stringstream htmlStream;
            bool inverser = checkInverserCouleur1->isChecked();
            htmlStream << "<html><head><style>"
                      << "body { background-color: black; margin: 0; padding: 10px; }"
                      << "pre { font-family: 'Courier New', monospace; font-size: 9px; line-height: 1; color: white; }"
                      << "</style></head><body><pre>";
            
            // Générer chaque pixel avec sa couleur (saut d'une ligne sur deux)
            for (int y = 0; y < imageCouleur.hauteur; y += 2) {
                for (int x = 0; x < imageCouleur.largeur; ++x) {
                    auto [r, g, b] = imageCouleur.pixelsRGB[y * imageCouleur.largeur + x];
                    
                    // Inverser les couleurs si demandé
                    if (inverser) {
                        r = 255 - r;
                        g = 255 - g;
                        b = 255 - b;
                    }
                    
                    // Trouver la couleur la plus proche dans la palette
                    int couleurIndex = trouverCouleurProche(r, g, b, paletteCoul);
                    const CouleurRGB& couleur = paletteCoul.couleurs[couleurIndex];
                    
                    htmlStream << "<span style=\"color:rgb(" << (int)couleur.r << "," << (int)couleur.g << "," << (int)couleur.b << ");\">"
                              << caractere.toStdString() << "</span>";
                }
                htmlStream << "\n";
            }
            
            htmlStream << "</pre></body></html>";
            apercu = QString::fromStdString(htmlStream.str());
            break;
        }

        case ModeCalibrage::COULEUR_MULTI: {
            // Mode couleur multi-caractères
            if (!imageCouleurChargee) {
                apercu = QString::fromUtf8("Chargement de l'image couleur...");
                break;
            }

            // Récupérer la palette ASCII (même switch que onChangerPalette)
            int asciiIndex = comboPaletteAscii->currentIndex();
            std::vector<std::string> paletteAscii;
            switch (asciiIndex) {
                case 0: paletteAscii = getPaletteNormale(); break;
                case 1: paletteAscii = getPaletteParDefaut(); break;
                case 2: paletteAscii = getPaletteClassiqueEtendue(); break;
                case 3: paletteAscii = getPaletteBlocs(); break;
                case 4: paletteAscii = getPaletteOmbreLumiere(); break;
                case 5: paletteAscii = getPaletteNatureLumiere(); break;
                case 6: paletteAscii = getPaletteMedieval(); break;
                case 7: paletteAscii = getPaletteDetailsFins(); break;
                case 8: paletteAscii = getPaletteAquarelle(); break;
                case 9: paletteAscii = getPaletteGradients(); break;
                case 10: paletteAscii = getPaletteCyberpunk(); break;
                case 11: paletteAscii = getPalette2Bit(); break;
                case 12: paletteAscii = getPaletteSaturation(); break;
                case 13: paletteAscii = getPaletteLettresSeules(); break;
                case 14: paletteAscii = getPaletteDetourage(); break;
                case 15: paletteAscii = getPaletteReflet(); break;
                case 16: paletteAscii = getPalettePoints(); break;
                case 17: paletteAscii = getPaletteLignes(); break;
                case 18: paletteAscii = getPaletteStippling(); break;
                case 19: paletteAscii = getPaletteDithering(); break;
                case 20: paletteAscii = getPaletteMinimaliste(); break;
                case 21: paletteAscii = getPaletteMonospace(); break;
                default: paletteAscii = getPaletteNormale();
            }

            // Récupérer la palette couleur
            int paletteIndex = comboPaletteCouleurMulti->currentIndex();
            PaletteCouleur paletteCoul;
            
            switch (paletteIndex) {
                case 0: // 1 couleur RGB
                    paletteCoul = getPaletteCouleur1(spinRed->value(), spinGreen->value(), spinBlue->value());
                    break;
                case 1: // 8 couleurs
                    paletteCoul = getPaletteCouleur8();
                    break;
                case 2: // 16 couleurs
                    paletteCoul = getPaletteCouleur16();
                    break;
                case 3: // 32 couleurs
                    paletteCoul = getPaletteCouleur32();
                    break;
                case 4: // 64 couleurs
                    paletteCoul = getPaletteCouleur64();
                    lblWarningCouleurMulti->setText(QString::fromUtf8("⚠ Calcul intensif en cours..."));
                    break;
                case 5: // 128 couleurs
                    paletteCoul = getPaletteCouleur128();
                    lblWarningCouleurMulti->setText(QString::fromUtf8("⚠ Très coûteux ! Soyez patient..."));
                    break;
                case 6: // 256 couleurs
                    paletteCoul = getPaletteCouleur256();
                    lblWarningCouleurMulti->setText(QString::fromUtf8("⚠ INSTABLE ! Peut crasher..."));
                    break;
                default:
                    paletteCoul = getPaletteCouleur16();
            }

            // Assigner la palette ASCII à la palette couleur
            paletteCoul.caracteres = paletteAscii;

            // Générer la preview HTML avec couleurs
            std::stringstream htmlStream;
            bool inverser = checkInverserCouleurMulti->isChecked();
            htmlStream << "<html><head><style>"
                      << "body { background-color: black; margin: 0; padding: 10px; }"
                      << "pre { font-family: 'Courier New', monospace; font-size: 9px; line-height: 1; color: white; }"
                      << "</style></head><body><pre>";
            
            // Générer chaque pixel avec caractère et couleur
            // Utiliser imageActuelle (PGM) pour le choix du caractère et imageCouleur (RGB) pour la couleur
            // Si inverser est coché, on inverse AUSSI l'image PGM comme en mode Simple
            ImagePGM imageAffichee = imageActuelle;
            if (inverser) {
                imageAffichee = inverserCouleurs(imageAffichee);
            }
            
            // Saut d'une ligne sur deux pour compenser le ratio 2:1 des caractères
            for (int y = 0; y < imageAffichee.hauteur; y += 2) {
                for (int x = 0; x < imageAffichee.largeur; ++x) {
                    // Récupérer le niveau de gris depuis imageAffichee pour le mapping caractère
                    unsigned char gris = imageAffichee.pixels[y * imageAffichee.largeur + x];
                    
                    // Mapper le niveau de gris au caractère (comme en mode Simple, SANS inverser charIndex)
                    int charIndex = static_cast<int>((gris / 255.0) * (paletteCoul.caracteres.size() - 1));
                    charIndex = std::max(0, std::min(charIndex, static_cast<int>(paletteCoul.caracteres.size()) - 1));
                    
                    // Récupérer la couleur depuis imageCouleur
                    auto [r, g, b] = imageCouleur.pixelsRGB[y * imageCouleur.largeur + x];
                    
                    // Trouver couleur proche dans la palette
                    int couleurIndex = trouverCouleurProche(r, g, b, paletteCoul);
                    const CouleurRGB& couleur = paletteCoul.couleurs[couleurIndex];
                    
                    htmlStream << "<span style=\"color:rgb(" << (int)couleur.r << "," << (int)couleur.g << "," << (int)couleur.b << ");\">"
                              << paletteCoul.caracteres[charIndex] << "</span>";
                }
                htmlStream << "\n";
            }
            
            htmlStream << "</pre></body></html>";
            apercu = QString::fromStdString(htmlStream.str());
            break;
        }
    }

    // Afficher selon le mode
    if (modeActuel == ModeCalibrage::SIMPLE) {
        // Complètement réinitialiser le QTextEdit pour effacer tout HTML
        textApercu->setDocument(new QTextDocument());
        textApercu->setStyleSheet("background-color: black; color: white; border: 1px solid #4A90E2;");
        textApercu->setFont(QFont("Courier", 8));
        textApercu->setPlainText(apercu);  // Force le mode texte brut
    } else {
        // Modes couleur : afficher HTML
        textApercu->setHtml(apercu);
    }
}

void FenetreCalibrage::onEnregistrer()
{
    QString nomBase = QFileInfo(cheminPGM).baseName();
    QString nomFichier;
    QString filtres;

    // Adapter le nom et les filtres selon le mode
    switch (modeActuel) {
        case ModeCalibrage::SIMPLE:
            nomFichier = QString("%1_ascii.txt").arg(nomBase);
            filtres = "Fichiers texte (*.txt);;Tous les fichiers (*)";
            break;
        
        case ModeCalibrage::COULEUR_1CARA:
        case ModeCalibrage::COULEUR_MULTI:
            nomFichier = QString("%1_ascii.html").arg(nomBase);
            filtres = "HTML (*.html);;ANSI (*.txt);;Tous les fichiers (*)";
            break;
    }

    QString selectedFilter;
    nomFichier = QFileDialog::getSaveFileName(this,
        QString::fromUtf8("Enregistrer l'ASCII Art"),
        QDir::homePath() + "/" + nomFichier,
        filtres,
        &selectedFilter);

    if (nomFichier.isEmpty()) {
        return;
    }

    // Adapter l'extension selon le filtre sélectionné pour les modes couleur
    if (modeActuel != ModeCalibrage::SIMPLE) {
        if (selectedFilter.contains("ANSI") && !nomFichier.endsWith(".txt", Qt::CaseInsensitive)) {
            // Remplacer .html par .txt si ANSI sélectionné
            if (nomFichier.endsWith(".html", Qt::CaseInsensitive)) {
                nomFichier.chop(5);
                nomFichier += ".txt";
            } else if (!nomFichier.contains('.')) {
                nomFichier += ".txt";
            }
        } else if (selectedFilter.contains("HTML") && !nomFichier.endsWith(".html", Qt::CaseInsensitive)) {
            // Remplacer .txt par .html si HTML sélectionné
            if (nomFichier.endsWith(".txt", Qt::CaseInsensitive)) {
                nomFichier.chop(4);
                nomFichier += ".html";
            } else if (!nomFichier.contains('.')) {
                nomFichier += ".html";
            }
        }
    }

    try {
        if (modeActuel == ModeCalibrage::SIMPLE) {
            // Mode classique : export TXT monochrome
            ImagePGM imageFinal = imageActuelle;
            if (checkInverser->isChecked()) {
                imageFinal = inverserCouleurs(imageFinal);
            }
            sauvegarderAsciiArt(imageFinal, paletteActuelle, nomFichier.toStdString());
        }
        else if (modeActuel == ModeCalibrage::COULEUR_1CARA) {
            // Mode couleur 1 cara : export HTML ou ANSI
            if (!imageCouleurChargee) {
                QMessageBox::warning(this, "Erreur", QString::fromUtf8("Image couleur non chargée"));
                return;
            }

            // Récupérer caractère et palette
            QString caractere;
            if (!lineCaractereCustom->text().isEmpty()) {
                caractere = lineCaractereCustom->text();
            } else {
                QString comboText = comboCaractere->currentText();
                caractere = (comboText.length() > 0) ? QString(comboText[0]) : "#";
            }

            int paletteIndex = comboPaletteCouleur1->currentIndex();
            PaletteCouleur paletteCoul;
            switch (paletteIndex) {
                case 0: paletteCoul = getPaletteCouleur1(spinRed->value(), spinGreen->value(), spinBlue->value()); break;
                case 1: paletteCoul = getPaletteCouleur8(); break;
                case 2: paletteCoul = getPaletteCouleur16(); break;
                case 3: paletteCoul = getPaletteCouleur32(); break;
                case 4: paletteCoul = getPaletteCouleur64(); break;
                case 5: paletteCoul = getPaletteCouleur128(); break;
                case 6: paletteCoul = getPaletteCouleur256(); break;
                default: paletteCoul = getPaletteCouleur16();
            }

            // Export selon l'extension
            if (nomFichier.endsWith(".html", Qt::CaseInsensitive)) {
                sauvegarderAsciiArtHTML(imageCouleur, paletteCoul, nomFichier.toStdString());
            } else {
                sauvegarderAsciiArtANSI(imageCouleur, paletteCoul, nomFichier.toStdString());
            }
        }
        else if (modeActuel == ModeCalibrage::COULEUR_MULTI) {
            // Mode couleur multi : export HTML ou ANSI
            if (!imageCouleurChargee) {
                QMessageBox::warning(this, "Erreur", QString::fromUtf8("Image couleur non chargée"));
                return;
            }

            // Récupérer palettes ASCII et couleur
            int asciiIndex = comboPaletteAscii->currentIndex();
            std::vector<std::string> paletteAscii;
            switch (asciiIndex) {
                case 0: paletteAscii = getPaletteNormale(); break;
                case 1: paletteAscii = getPaletteParDefaut(); break;
                case 2: paletteAscii = getPaletteClassiqueEtendue(); break;
                case 3: paletteAscii = getPaletteBlocs(); break;
                case 4: paletteAscii = getPaletteSaturation(); break;
                default: paletteAscii = getPaletteNormale();
            }

            int paletteIndex = comboPaletteCouleurMulti->currentIndex();
            PaletteCouleur paletteCoul;
            switch (paletteIndex) {
                case 0: paletteCoul = getPaletteCouleur8(); break;
                case 1: paletteCoul = getPaletteCouleur16(); break;
                case 2: paletteCoul = getPaletteCouleur32(); break;
                case 3: paletteCoul = getPaletteCouleur64(); break;
                case 4: paletteCoul = getPaletteCouleur128(); break;
                case 5: paletteCoul = getPaletteCouleur256(); break;
                default: paletteCoul = getPaletteCouleur16();
            }

            paletteCoul.caracteres = paletteAscii;

            // Export selon l'extension
            if (nomFichier.endsWith(".html", Qt::CaseInsensitive)) {
                sauvegarderAsciiArtHTML(imageCouleur, paletteCoul, nomFichier.toStdString());
            } else {
                sauvegarderAsciiArtANSI(imageCouleur, paletteCoul, nomFichier.toStdString());
            }
        }

        QMessageBox::information(this, QString::fromUtf8("Succès"),
            QString::fromUtf8("ASCII Art enregistré avec succès :\n%1").arg(nomFichier));

        lblStatut->setText(QString::fromUtf8("✓ Fichier enregistré : ") + QFileInfo(nomFichier).fileName());
        lblStatut->setStyleSheet("color: #27AE60; font-weight: bold; padding: 10px; background-color: #2B2B2B;");

    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Erreur",
            QString("Erreur lors de l'enregistrement : %1").arg(e.what()));
        lblStatut->setText(QString::fromUtf8("✗ Erreur lors de l'enregistrement"));
        lblStatut->setStyleSheet("color: #E74C3C; font-weight: bold; padding: 10px; background-color: #2B2B2B;");
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

void FenetreCalibrage::onModeChange()
{
    int modeId = groupMode->checkedId();
    modeActuel = static_cast<ModeCalibrage>(modeId);

    // Changer le panel affiché
    stackedPanels->setCurrentIndex(static_cast<int>(modeActuel));

    // Lazy loading de l'image couleur
    if ((modeActuel == ModeCalibrage::COULEUR_1CARA || modeActuel == ModeCalibrage::COULEUR_MULTI) 
        && !imageCouleurChargee) {
        try {
            lblStatut->setText(QString::fromUtf8("⏳ Chargement de l'image couleur..."));
            lblStatut->setStyleSheet("color: #FFA500; font-weight: bold; padding: 10px; background-color: #2B2B2B;");
            QApplication::processEvents();

            imageCouleurOriginale = chargerImageCouleur(cheminImageOriginale.toStdString(), true);
            
            // Redimensionner l'image couleur pour correspondre aux dimensions actuelles
            imageCouleur = redimensionnerImageCouleur(imageCouleurOriginale, imageActuelle.largeur, imageActuelle.hauteur);
            
            imageCouleurChargee = true;

            lblStatut->setText(QString::fromUtf8("✓ Image couleur chargée"));
            lblStatut->setStyleSheet("color: #27AE60; font-weight: bold; padding: 10px; background-color: #2B2B2B;");
        }
        catch (const std::exception& e) {
            QMessageBox::critical(this, "Erreur", 
                QString::fromUtf8("Impossible de charger l'image couleur : %1").arg(e.what()));
            // Revenir au mode Simple
            radioSimple->setChecked(true);
            modeActuel = ModeCalibrage::SIMPLE;
            stackedPanels->setCurrentIndex(0);
            return;
        }
    }

    // Actualiser l'aperçu
    actualiserApercu();
}