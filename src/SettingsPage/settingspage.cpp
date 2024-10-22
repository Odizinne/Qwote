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
}

SettingsPage::~SettingsPage()
{
    emit closed();
    delete ui;
}

void SettingsPage::populateFontComboBox() {
    QFontDatabase fontDatabase;
    QStringList fonts = fontDatabase.families();

    ui->fontComboBox->clear();

    foreach (const QString &font, fonts) {
        ui->fontComboBox->addItem(font);
    }
}

void SettingsPage::onFontComboBoxIndexChanged() {
    saveSettings();
    emit fontChanged();
}

void SettingsPage::loadSettings()
{
    QDir settingsDir(QFileInfo(settingsFile).absolutePath());
    if (!settingsDir.exists()) {
        settingsDir.mkpath(settingsDir.absolutePath());
    }

    QFile file(settingsFile);
    if (!file.exists()) {
        ui->fontComboBox->setCurrentText("Consolas");

    } else {
        if (file.open(QIODevice::ReadOnly)) {
            QJsonParseError parseError;
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                settings = doc.object();
                ui->fontComboBox->setCurrentText(settings.value("font").toString());
            }
            file.close();
        }
    }
}

void SettingsPage::saveSettings()
{
    settings["font"] = ui->fontComboBox->currentText();

    QFile file(settingsFile);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(settings);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }
}
