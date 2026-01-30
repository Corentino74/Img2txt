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
    , imagePreviewActive(false)
    , imageValide(false)
{
    // Activer le drag & drop
    setAcceptDrops(true);
    dossierImages = QDir::currentPath() + "/images";
    dossierRendus = QDir::currentPath() + "/rendus";
    dossierTemp = creerDossierTemp();

    QDir().mkpath(dossierImages);
    QDir().mkpath(dossierRendus);

    setupUI();
    chargerListeImages();
}

FenetreChargement::~FenetreChargement()
{
}

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
    setWindowTitle(QString::fromUtf8("Img2txt - version 3.5 : PLUS DE PALETTES !"));
    resize(1000, 600);

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setSpacing(0);
    layoutPrincipal->setContentsMargins(0, 0, 0, 0);

    // === TITRE ===
    QLabel* titre = new QLabel(QString::fromUtf8("Img2txt V3.5"));
    titre->setAlignment(Qt::AlignCenter);
    titre->setStyleSheet("font-size: 18px; font-weight: bold; padding: 15px; background-color: #2B2B2B; color: white;");
    layoutPrincipal->addWidget(titre);

    // === ZONE CENTRALE: Layout horizontal (Liste + Preview) ===
    QWidget* zoneCentrale = new QWidget();
    QHBoxLayout* layoutCentral = new QHBoxLayout(zoneCentrale);
    layoutCentral->setSpacing(0);
    layoutCentral->setContentsMargins(0, 0, 0, 0);

    // --- PANNEAU GAUCHE: Liste d'images ---
    QWidget* panneauListe = new QWidget();
    panneauListe->setMinimumWidth(350);
    panneauListe->setMaximumWidth(450);
    panneauListe->setStyleSheet("background-color: #3C3F41;");
    QVBoxLayout* layoutListe = new QVBoxLayout(panneauListe);

    QLabel* titreListe = new QLabel(QString::fromUtf8("Images disponibles dans le dossier 'images'"));
    titreListe->setStyleSheet("font-weight: bold; color: white; padding: 10px;");
    layoutListe->addWidget(titreListe);

    listeImages = new QListWidget();
    listeImages->setSelectionMode(QAbstractItemView::SingleSelection);
    listeImages->setStyleSheet("background-color: #2B2B2B; color: white; border: none; padding: 5px;");
    connect(listeImages, &QListWidget::itemDoubleClicked, this, &FenetreChargement::onImageListeClicked);
    layoutListe->addWidget(listeImages);

    QLabel* instruction = new QLabel(QString::fromUtf8("Double-cliquez pour convertir"));
    instruction->setStyleSheet("color: #9E9E9E; font-style: italic; padding: 5px;");
    layoutListe->addWidget(instruction);

    layoutCentral->addWidget(panneauListe);

    // --- PANNEAU DROIT: Preview/Drop zone ---
    QWidget* panneauPreview = new QWidget();
    panneauPreview->setStyleSheet("background-color: #2B2B2B;");
    QVBoxLayout* layoutPreview = new QVBoxLayout(panneauPreview);

    lblPreview = new QLabel();
    lblPreview->setAlignment(Qt::AlignCenter);
    lblPreview->setStyleSheet("border: 2px dashed #4A90E2; background-color: #3C3F41; color: #9E9E9E;");
    lblPreview->setMinimumSize(400, 400);
    lblPreview->setScaledContents(false);
    resetPreview();  // Afficher le texte par défaut
    layoutPreview->addWidget(lblPreview, 1);

    lblInfo = new QLabel(QString::fromUtf8("Aucune image chargée"));
    lblInfo->setStyleSheet("padding: 10px; background-color: #3C3F41; color: white; border-radius: 5px;");
    lblInfo->setAlignment(Qt::AlignCenter);
    layoutPreview->addWidget(lblInfo);

    layoutCentral->addWidget(panneauPreview, 1);

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

    // Bouton "?" - Aide
    btnAide = new QPushButton();
    btnAide->setIcon(QIcon(":/illustrations/aide.png"));
    btnAide->setIconSize(QSize(32, 32));
    btnAide->setMinimumSize(60, 50);
    btnAide->setToolTip(QString::fromUtf8("Aide"));
    btnAide->setStyleSheet("QPushButton { background-color: #778aa0; border-radius: 5px; } QPushButton:hover { background-color: #8697a8; }");
    connect(btnAide, &QPushButton::clicked, this, &FenetreChargement::onAide);
    layoutBoutons->addWidget(btnAide);

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

    // Bouton "Convertir" - Convertir en texte ASCII
    btnConvertir = new QPushButton(QString::fromUtf8("  Convertir en texte ASCII =>"));
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
    imagePreviewActive = false;
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
    imagePreviewActive = true;
}

void FenetreChargement::chargerListeImages()
{
    listeImages->clear();

    QDir dir(dossierImages);
    QStringList filtres;
    filtres << "*.png" << "*.jpg" << "*.jpeg" << "*.PNG" << "*.JPG" << "*.JPEG";

    QFileInfoList fichiers = dir.entryInfoList(filtres, QDir::Files);

    if (fichiers.isEmpty()) {
        QListWidgetItem* item = new QListWidgetItem(QString::fromUtf8("Aucune image trouvée dans le dossier 'images'"));
        item->setFlags(Qt::NoItemFlags);
        item->setForeground(QColor("#9E9E9E"));
        listeImages->addItem(item);
    }
    else {
        foreach(const QFileInfo & fichier, fichiers) {
            listeImages->addItem(fichier.fileName());
        }
    }
}

// Charger une image (PNG/JPG/PGM)
void FenetreChargement::onChargerPGM()
{
    QString fichier = QFileDialog::getOpenFileName(this,
        QString::fromUtf8("Sélectionner une image"),
        dossierImages,
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

void FenetreChargement::onImageListeClicked(QListWidgetItem* item)
{
    if (!item || item->flags() == Qt::NoItemFlags) {
        return;
    }

    QString nomFichier = item->text();
    QString cheminComplet = dossierImages + "/" + nomFichier;

    convertirEtChargerImage(cheminComplet);
}

void FenetreChargement::onConvertirManuel()
{
    // Cette méthode n'est plus utilisée (fusionnée avec onChargerPGM)
    onChargerPGM();
}

// ✨ NOUVEAU: Afficher l'aide
void FenetreChargement::onAide()
{
    QMessageBox::information(this,
        QString::fromUtf8("Aide - Img2txt 3.0"),
        QString::fromUtf8(
            "<h3>Comment utiliser Img2txt ?</h3>"
            "<p><b>1. Charger une image :</b></p>"
            "<ul>"
            "<li>Double-cliquez sur une image de la liste</li>"
            "<li>Glissez-déposez une image dans la zone de preview</li>"
            "<li>Cliquez sur 'Charger' pour sélectionner un fichier</li>"
            "</ul>"
            "<p><b>2. Convertir :</b></p>"
            "<ul>"
            "<li>Une fois l'image chargée, cliquez sur 'Convertir =>'</li>"
            "<li>Vous accéderez à la fenêtre de calibrage</li>"
            "</ul>"
            "<p><b>3. Formats supportés :</b></p>"
            "<ul>"
            "<li>PNG, JPG, JPEG (convertis automatiquement)</li>"
            "<li>PGM (chargement direct)</li>"
            "</ul>"
            "<p><b>Astuce :</b> Le drag & drop est le moyen le plus rapide !</p>"
        ));
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
        emit imagePGMChargee(imageChargee, cheminPGM);
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
        if (!imagePreviewActive) {
            lblPreview->setText(QString::fromUtf8("Déposez l'image ici !"));
        }
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
