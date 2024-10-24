#include "settingspage.h"
#include "ui_settingspage.h"
#include <QFile>
#include <QJsonDocument>
#include <QDir>
#include <QFontDatabase>
#include <QStandardPaths>

#ifdef _WIN32
const QString SettingsPage::settingsFile = QStandardPaths::writableLocation(
                                               QStandardPaths::AppDataLocation)
                                           + "/Qwote/settings.json";
#elif __linux__
const QString SettingsPage::settingsFile = QDir::homePath() + "/.config/Qwote/settings.json";

#endif

SettingsPage::SettingsPage(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SettingsPage)
{
    ui->setupUi(this);
    populateFontComboBox();
    loadSettings();

    connect(ui->fontComboBox, &QComboBox::currentIndexChanged, this, &SettingsPage::onFontComboBoxIndexChanged);
    connect(ui->opacitySlider, &QSlider::valueChanged, this, &SettingsPage::onOpacitySliderValueChanged);
    connect(ui->roundedCornersCheckbox, &QCheckBox::checkStateChanged, this, &SettingsPage::onRoundedCornersCheckboxStateChanged);
}

SettingsPage::~SettingsPage()
{
    emit closed();
    delete ui;
}

void SettingsPage::populateFontComboBox() {
    QStringList fonts = QFontDatabase::families();

    ui->fontComboBox->clear();

    foreach (const QString &font, fonts) {
        ui->fontComboBox->addItem(font);
    }
}

void SettingsPage::onFontComboBoxIndexChanged() {
    saveSettings();
    emit settingsChanged();
}

void SettingsPage::loadSettings()
{
    QDir settingsDir(QFileInfo(settingsFile).absolutePath());
    if (!settingsDir.exists()) {
        settingsDir.mkpath(settingsDir.absolutePath());
    }

    QFile file(settingsFile);
    if (!file.exists()) {
#ifdef _WIN32
        ui->fontComboBox->setCurrentText("Consolas");
#elif __linux__
        ui->fontComboBox->setCurrentText("Monospace");
#endif

    } else {
        if (file.open(QIODevice::ReadOnly)) {
            QJsonParseError parseError;
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                settings = doc.object();
                ui->fontComboBox->setCurrentText(settings.value("font").toString());
                ui->opacitySlider->setValue(settings.value("opacity").toInt());
                ui->roundedCornersCheckbox->setChecked(settings.value("roundedCorners").toBool());
            }
            file.close();
        }
    }
}

void SettingsPage::saveSettings()
{
    settings["font"] = ui->fontComboBox->currentText();
    settings["opacity"] = ui->opacitySlider->value();
    settings["roundedCorners"] = ui->roundedCornersCheckbox->isChecked();

    QFile file(settingsFile);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(settings);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }
}

void SettingsPage::onOpacitySliderValueChanged() {
    saveSettings();
    emit settingsChanged();
}

void SettingsPage::onRoundedCornersCheckboxStateChanged() {
    saveSettings();
    emit settingsChanged();
}
