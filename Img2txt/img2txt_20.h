#pragma once

#include <QtWidgets/QMainWindow>
#include <QStackedWidget>
#include "ui_Img2txt_20.h"
#include "FenetreChargement.h"
#include "FenetreCalibrage.h"

class Img2txt_20 : public QMainWindow
{
    Q_OBJECT

public:
    Img2txt_20(QWidget* parent = nullptr);
    ~Img2txt_20();

private slots:
    void onImageChargee(const ImagePGM& image, const QString& chemin);
    void onRetourChargement();

private:
    Ui::Img2txt_20Class ui;
    QStackedWidget* stackedWidget;
    FenetreChargement* fenetreChargement;
    FenetreCalibrage* fenetreCalibrage;
};

