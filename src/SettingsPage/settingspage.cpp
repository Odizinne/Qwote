#include "settingspage.h"
#include "ui_settingspage.h"
#include <QFontDatabase>
#include <QSettings>

SettingsPage::SettingsPage(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SettingsPage)
{
    ui->setupUi(this);
    populateFontComboBox();
    loadSettings();

    connect(ui->fontComboBox, &QComboBox::currentIndexChanged, this, &SettingsPage::saveSettings);
    connect(ui->opacitySlider, &QSlider::valueChanged, this, &SettingsPage::saveSettings);
    connect(ui->roundedCornersCheckbox, &QCheckBox::stateChanged, this, &SettingsPage::saveSettings);
    connect(ui->frameCheckbox, &QCheckBox::stateChanged, this, &SettingsPage::saveSettings);
}

SettingsPage::~SettingsPage()
{
    emit closed();
    delete ui;
}

void SettingsPage::populateFontComboBox() {
    QStringList fonts = QFontDatabase::families();
    ui->fontComboBox->clear();
    for (const QString &font : fonts) {
        ui->fontComboBox->addItem(font);
    }
}

void SettingsPage::loadSettings()
{
    QSettings settings("Odizinne", "Qwote");

    QString defaultFont;
#ifdef _WIN32
    defaultFont = "Consolas";
#elif __linux__
    defaultFont = "Monospace";
#endif

    ui->fontComboBox->setCurrentText(settings.value("font", defaultFont).toString());
    ui->opacitySlider->setValue(settings.value("opacity", 128).toInt() + 127);
    ui->roundedCornersCheckbox->setChecked(settings.value("roundedCorners", true).toBool());
    ui->frameCheckbox->setChecked(settings.value("frame", true).toBool());
}

void SettingsPage::saveSettings()
{
    QSettings settings("Odizinne", "Qwote");

    settings.setValue("font", ui->fontComboBox->currentText());
    settings.setValue("opacity", ui->opacitySlider->value() + 127);
    settings.setValue("roundedCorners", ui->roundedCornersCheckbox->isChecked());
    settings.setValue("frame", ui->frameCheckbox->isChecked());
    qDebug() << ui->opacitySlider->value();

    emit settingsChanged();
}
