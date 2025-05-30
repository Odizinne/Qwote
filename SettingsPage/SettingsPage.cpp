#include "SettingsPage.h"
#include "ui_SettingsPage.h"
#include <QFontDatabase>
#include <QSettings>

SettingsPage::SettingsPage(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SettingsPage)
{
    ui->setupUi(this);
    populateFontComboBox();
    populateColorComboBox();
    loadSettings();

    connect(ui->fontComboBox, &QComboBox::currentIndexChanged, this, &SettingsPage::saveSettings);
    connect(ui->colorComboBox, &QComboBox::currentIndexChanged, this, &SettingsPage::saveSettings);
    connect(ui->opacitySlider, &QSlider::valueChanged, this, &SettingsPage::saveSettings);
    connect(ui->roundedCornersCheckbox, &QCheckBox::stateChanged, this, &SettingsPage::saveSettings);
    connect(ui->frameCheckbox, &QCheckBox::stateChanged, this, &SettingsPage::saveSettings);
}

SettingsPage::~SettingsPage()
{
    emit closed();
    delete ui;
}

void SettingsPage::populateFontComboBox()
{
    QStringList fonts = QFontDatabase::families();
    ui->fontComboBox->clear();
    for (const QString &font : fonts) {
        ui->fontComboBox->addItem(font);
    }
}

void SettingsPage::populateColorComboBox()
{
#ifdef _WIN32
    ui->colorComboBox->addItem(tr("Windows"));
#endif
    ui->colorComboBox->addItem(tr("Orange"));
    ui->colorComboBox->addItem(tr("Red"));
    ui->colorComboBox->addItem(tr("Blue"));
    ui->colorComboBox->addItem(tr("Green"));
    ui->colorComboBox->addItem(tr("Teal"));
    ui->colorComboBox->addItem(tr("Pink"));
    ui->colorComboBox->addItem(tr("Purple"));
    ui->colorComboBox->addItem(tr("Yellow"));
}

void SettingsPage::loadSettings()
{
    QSettings settings("Odizinne", "Qwote");

    QString defaultFont;
    QString defaultColor;
#ifdef _WIN32
    defaultFont = "Consolas";
    defaultColor = "Windows";
#elif __linux__
    defaultFont = "Monospace";
    defaultColor = "Orange";
#endif

    ui->fontComboBox->setCurrentText(settings.value("font", defaultFont).toString());
    ui->colorComboBox->setCurrentText(settings.value("color", defaultColor).toString());
    ui->opacitySlider->setValue(settings.value("opacity", 255).toInt());
    ui->roundedCornersCheckbox->setChecked(settings.value("roundedCorners", true).toBool());
    ui->frameCheckbox->setChecked(settings.value("frame", true).toBool());
}

void SettingsPage::saveSettings()
{
    QSettings settings("Odizinne", "Qwote");

    settings.setValue("font", ui->fontComboBox->currentText());
    settings.setValue("color", ui->colorComboBox->currentText());
    settings.setValue("opacity", ui->opacitySlider->value());
    settings.setValue("roundedCorners", ui->roundedCornersCheckbox->isChecked());
    settings.setValue("frame", ui->frameCheckbox->isChecked());

    emit settingsChanged();
}
