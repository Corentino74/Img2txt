#include "FenetreChargement.h"
#include <QGroupBox>
#include <QApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QSplitter>
#include <QTemporaryDir>

FenetreChargement::FenetreChargement(QWidget* parent)
    : QWidget(parent)
    , imageValide(false)
{
    // Activer le drag & drop
    setAcceptDrops(true);
    dossierTemp = creerDossierTemp();

    setupUI();
}

FenetreChargement::~FenetreChargement() {}

// Créer le dossier temporaire pour les PGM
QString FenetreChargement::creerDossierTemp()
{
    QString tempPath = QDir::tempPath() + "/img2txt";
    QDir().mkpath(tempPath);
    return tempPath;
}

// Nettoyer les fichiers temporaires (appelé au démarrage et à la fermeture)
void FenetreChargement::nettoyerFichiersTemporaires()
{
    QString tempPath = QDir::tempPath() + "/img2txt";
    QDir tempDir(tempPath);
    if (tempDir.exists()) {
        tempDir.removeRecursively();
    }
}

void FenetreChargement::setupUI()
{
    setWindowTitle(QString::fromUtf8("Img2txt V4.0 - ASCII Art Converter"));
    resize(900, 650);

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setSpacing(0);
    layoutPrincipal->setContentsMargins(0, 0, 0, 0);

    // === TITRE ===
    QLabel* titre = new QLabel(QString::fromUtf8("Img2txt V4.0"));
    titre->setAlignment(Qt::AlignCenter);
    titre->setStyleSheet("font-size: 18px; font-weight: bold; padding: 15px; background-color: #2B2B2B; color: white;");
    layoutPrincipal->addWidget(titre);

    // === ZONE CENTRALE: Preview + Aide ===
    QWidget* zoneCentrale = new QWidget();
    zoneCentrale->setStyleSheet("background-color: #2B2B2B;");
    QVBoxLayout* layoutCentral = new QVBoxLayout(zoneCentrale);
    layoutCentral->setSpacing(15);
    layoutCentral->setContentsMargins(20, 20, 20, 20);

    // Aide inline
    QLabel* lblAide = new QLabel(QString::fromUtf8(
        "<b style='color:#4A90E2;'>Guide rapide :</b><br>"
        "<span style='color:#9E9E9E;'>"
        "\u2022 Cliquez sur 'Charger' pour s\u00e9lectionner une image<br>"
        "\u2022 Ou glissez-d\u00e9posez un fichier dans la zone ci-dessous<br>"
        "\u2022 Formats support\u00e9s : PNG, JPG, JPEG, PGM<br>"
        "\u2022 Cliquez sur 'Convertir' pour ouvrir la fen\u00eatre de calibrage</span>"
    ));
    lblAide->setWordWrap(true);
    lblAide->setStyleSheet("padding: 15px; background-color: #3C3F41; border-radius: 5px;");
    layoutCentral->addWidget(lblAide);

    // Zone de preview/drop
    lblPreview = new QLabel();
    lblPreview->setAlignment(Qt::AlignCenter);
    lblPreview->setStyleSheet("border: 2px dashed #4A90E2; background-color: #3C3F41; color: #9E9E9E;");
    lblPreview->setMinimumSize(500, 400);
    lblPreview->setScaledContents(false);
    resetPreview();
    layoutCentral->addWidget(lblPreview, 1);

    // Info image
    lblInfo = new QLabel(QString::fromUtf8("Aucune image charg\u00e9e"));
    lblInfo->setStyleSheet("padding: 10px; background-color: #3C3F41; color: white; border-radius: 5px;");
    lblInfo->setAlignment(Qt::AlignCenter);
    layoutCentral->addWidget(lblInfo);

    layoutPrincipal->addWidget(zoneCentrale, 1);

    // === BARRE DE STATUT ===
    lblStatut = new QLabel("");
    lblStatut->setStyleSheet("color: #27AE60; font-weight: bold; padding: 10px; background-color: #2B2B2B;");
    lblStatut->setAlignment(Qt::AlignCenter);
    layoutPrincipal->addWidget(lblStatut);

    // === BARRE DE BOUTONS EN BAS ===
    QWidget* barreBoutons = new QWidget();
    barreBoutons->setStyleSheet("background-color: #2B2B2B; padding: 10px;");
    QHBoxLayout* layoutBoutons = new QHBoxLayout(barreBoutons);
    layoutBoutons->setSpacing(10);

    // Bouton "Charger" - Charger une image ou un pgm
    btnCharger = new QPushButton(QString::fromUtf8("  Charger une image ou un pgm"));
    btnCharger->setIcon(QIcon(":/illustrations/folder.png"));
    btnCharger->setIconSize(QSize(32, 32));
    btnCharger->setMinimumHeight(50);
    btnCharger->setStyleSheet("QPushButton { background-color: #4A90E2; color: white; font-weight: bold; border-radius: 5px; text-align: left; padding-left: 10px; } QPushButton:hover { background-color: #357ABD; }");
    connect(btnCharger, &QPushButton::clicked, this, &FenetreChargement::onChargerPGM);
    layoutBoutons->addWidget(btnCharger);

    // Bouton "Supprimer" - Supprimer l'image actuelle
    btnSupprimer = new QPushButton(QString::fromUtf8("  Supprimer l'image actuelle"));
    btnSupprimer->setIcon(QIcon(":/illustrations/bin.png"));
    btnSupprimer->setIconSize(QSize(32, 32));
    btnSupprimer->setMinimumHeight(50);
    btnSupprimer->setEnabled(false);  // Désactivé par défaut
    btnSupprimer->setStyleSheet("QPushButton { background-color: #E74C3C; color: white; font-weight: bold; border-radius: 5px; text-align: left; padding-left: 10px; } QPushButton:hover { background-color: #C0392B; } QPushButton:disabled { background-color: #7F8C8D; }");
    connect(btnSupprimer, &QPushButton::clicked, this, &FenetreChargement::onSupprimer);
    layoutBoutons->addWidget(btnSupprimer);

    // Bouton "Convertir" - Ouvre FenetreCalibrage
    btnConvertir = new QPushButton(QString::fromUtf8("  Convertir"));
    btnConvertir->setIcon(QIcon(":/illustrations/box.png"));
    btnConvertir->setIconSize(QSize(32, 32));
    btnConvertir->setMinimumHeight(50);
    btnConvertir->setEnabled(false);  // Désactivé par défaut
    btnConvertir->setStyleSheet("QPushButton { background-color: #27AE60; color: white; font-weight: bold; border-radius: 5px; text-align: left; padding-left: 10px; } QPushButton:hover { background-color: #229954; } QPushButton:disabled { background-color: #7F8C8D; }");
    connect(btnConvertir, &QPushButton::clicked, this, &FenetreChargement::onConvertir);
    layoutBoutons->addWidget(btnConvertir);

    layoutPrincipal->addWidget(barreBoutons);
}

// Réinitialiser la preview (afficher le texte par défaut)
void FenetreChargement::resetPreview()
{
    lblPreview->clear();
    lblPreview->setText(QString::fromUtf8("glissez-déposez votre image ici..."));
    lblPreview->setStyleSheet("border: 2px dashed #4A90E2; background-color: #3C3F41; color: #9E9E9E; font-size: 16px; font-style: italic;");
}

// Afficher la preview de l'image chargée
void FenetreChargement::afficherPreview()
{
    if (cheminImageOriginale.isEmpty()) {
        resetPreview();
        return;
    }

    QPixmap pixmap(cheminImageOriginale);
    if (pixmap.isNull()) {
        resetPreview();
        return;
    }

    // Redimensionner l'image pour qu'elle rentre dans le label tout en gardant le ratio
    QPixmap scaledPixmap = pixmap.scaled(lblPreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    lblPreview->setPixmap(scaledPixmap);
    lblPreview->setStyleSheet("border: 2px solid #27AE60; background-color: #2B2B2B;");
}

// Charger une image (PNG/JPG/PGM)
void FenetreChargement::onChargerPGM()
{
    QString fichier = QFileDialog::getOpenFileName(this,
        QString::fromUtf8("Sélectionner une image"),
        QDir::homePath(),
        "Images (*.png *.jpg *.jpeg *.pgm);;Fichiers PGM (*.pgm);;Images PNG/JPG (*.png *.jpg *.jpeg);;Tous les fichiers (*)");

    if (fichier.isEmpty()) {
        return;
    }

    QString ext = QFileInfo(fichier).suffix().toLower();

    if (ext == "pgm") {
        // Charger directement le PGM
        try {
            imageChargee = lireFichierPGM(fichier.toStdString());
            cheminPGM = fichier;
            cheminImageOriginale = fichier;
            imageValide = true;

            afficherInfoImage();
            afficherPreview();
            btnSupprimer->setEnabled(true);
            btnConvertir->setEnabled(true);

            lblStatut->setText(QString::fromUtf8("✓ Image PGM chargée avec succès"));
            lblStatut->setStyleSheet("color: #27AE60; font-weight: bold; padding: 10px; background-color: #2B2B2B;");

        }
        catch (const std::exception& e) {
            QMessageBox::critical(this, "Erreur", QString("Erreur lors du chargement : %1").arg(e.what()));
            imageValide = false;
            btnSupprimer->setEnabled(false);
            btnConvertir->setEnabled(false);
        }
    }
    else {
        // Convertir PNG/JPG en PGM
        convertirEtChargerImage(fichier);
    }
}

// Supprimer l'image chargée
void FenetreChargement::onSupprimer()
{
    // Réinitialiser tout
    imageValide = false;
    cheminPGM.clear();
    cheminImageOriginale.clear();
    imageChargee = ImagePGM();

    // Reset UI
    resetPreview();
    lblInfo->setText(QString::fromUtf8("Aucune image chargée"));
    lblStatut->setText(QString::fromUtf8("Image supprimée"));
    lblStatut->setStyleSheet("color: #9E9E9E; font-weight: bold; padding: 10px; background-color: #2B2B2B;");

    btnSupprimer->setEnabled(false);
    btnConvertir->setEnabled(false);
}

// ✨ NOUVEAU: Convertir => Passage à la fenêtre de calibrage
void FenetreChargement::onConvertir()
{
    if (imageValide) {
        emit imagePGMChargee(imageChargee, cheminPGM, cheminImageOriginale);
    }
}

void FenetreChargement::afficherInfoImage()
{
    QString info = QString::fromUtf8("<b>Image chargée :</b><br>"
        "Dimensions : %1 x %2 pixels<br>"
        "Fichier : %3")
        .arg(imageChargee.largeur)
        .arg(imageChargee.hauteur)
        .arg(QFileInfo(cheminImageOriginale).fileName());
    lblInfo->setText(info);
    lblInfo->setStyleSheet("padding: 10px; background-color: #3C3F41; color: white; border-radius: 5px;");
}

// Fonction utilitaire pour convertir et charger une image
void FenetreChargement::convertirEtChargerImage(const QString& cheminImage)
{
    try {
        lblStatut->setText(QString::fromUtf8("⏳ Conversion en cours..."));
        lblStatut->setStyleSheet("color: #FFA500; font-weight: bold; padding: 10px; background-color: #2B2B2B;");
        QApplication::processEvents();

        // Charger l'image originale
        imageChargee = chargerImage(cheminImage.toStdString());
        cheminImageOriginale = cheminImage;

        // Sauvegarder dans le dossier temporaire au lieu de dossier images
        QFileInfo info(cheminImage);
        QString nomPGM = info.baseName() + ".pgm";
        cheminPGM = dossierTemp + "/" + nomPGM;
        sauvegarderEnPGM(imageChargee, cheminPGM.toStdString());

        imageValide = true;
        afficherInfoImage();
        afficherPreview();
        btnSupprimer->setEnabled(true);
        btnConvertir->setEnabled(true);

        lblStatut->setText(QString::fromUtf8("✓ Image convertie : %1").arg(info.fileName()));
        lblStatut->setStyleSheet("color: #27AE60; font-weight: bold; padding: 10px; background-color: #2B2B2B;");

    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Erreur", QString("Erreur : %1").arg(e.what()));
        imageValide = false;
        btnSupprimer->setEnabled(false);
        btnConvertir->setEnabled(false);
        lblStatut->setText(QString::fromUtf8("✗ Échec de la conversion"));
        lblStatut->setStyleSheet("color: #E74C3C; font-weight: bold; padding: 10px; background-color: #2B2B2B;");
    }
}

// ✨ Gestion du drag & drop
void FenetreChargement::dragEnterEvent(QDragEnterEvent* event)
{
    // Accepter uniquement si l'événement contient des URLs de fichiers
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
        lblPreview->setStyleSheet("border: 2px dashed #27AE60; background-color: #3C3F41; color: #27AE60; font-size: 16px; font-style: italic;");
        lblPreview->setText(QString::fromUtf8("Déposez l'image ici !"));
    }
}

void FenetreChargement::dropEvent(QDropEvent* event)
{
    const QMimeData* mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        QList<QUrl> urls = mimeData->urls();
        if (!urls.isEmpty()) {
            QString cheminFichier = urls.first().toLocalFile();

            // Vérifier l'extension du fichier
            QString ext = QFileInfo(cheminFichier).suffix().toLower();

            if (ext == "pgm") {
                // Charger directement le PGM
                try {
                    imageChargee = lireFichierPGM(cheminFichier.toStdString());
                    cheminPGM = cheminFichier;
                    cheminImageOriginale = cheminFichier;
                    imageValide = true;

                    afficherInfoImage();
                    afficherPreview();
                    btnSupprimer->setEnabled(true);
                    btnConvertir->setEnabled(true);


                    lblStatut->setText(QString::fromUtf8("✓ Image PGM chargée par drag & drop"));
                    lblStatut->setStyleSheet("color: #27AE60; font-weight: bold; padding: 10px; background-color: #2B2B2B;");

                }
                catch (const std::exception& e) {
                    QMessageBox::critical(this, "Erreur", QString("Erreur : %1").arg(e.what()));
                    resetPreview();
                }
            }
            else if (ext == "png" || ext == "jpg" || ext == "jpeg") {
                // Convertir l'image
                convertirEtChargerImage(cheminFichier);
            }
            else {
                QMessageBox::warning(this, "Format non supporté",
                    "Seuls les formats PNG, JPG, JPEG et PGM sont acceptés.");
                resetPreview();
            }
        }
        event->acceptProposedAction();
    }
}
