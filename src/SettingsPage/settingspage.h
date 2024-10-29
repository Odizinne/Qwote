#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QMainWindow>

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
    void populateFontComboBox();

private slots:
    void saveSettings();

signals:
    void settingsChanged();
    void closed();
};

#endif // SETTINGSPAGE_H
