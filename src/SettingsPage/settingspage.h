#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QMainWindow>
#include <QJsonObject>

namespace Ui {
class SettingsPage;
}

class SettingsPage : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingsPage(QWidget *parent = nullptr);
    ~SettingsPage();

private:
    Ui::SettingsPage *ui;
    void loadSettings();
    void saveSettings();
    QJsonObject settings;
    static const QString settingsFile;
    void populateFontComboBox();

private slots:
    void onFontComboBoxIndexChanged();
    void onOpacitySliderValueChanged();

signals:
    void fontChanged();
    void closed();
    void opacityChanged();
};

#endif // SETTINGSPAGE_H
