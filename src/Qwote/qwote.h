#ifndef QWOTE_H
#define QWOTE_H

#include <QWidget>
#include <QSystemTrayIcon>
#include "settingspage.h"
// Remove the direct include of notewidget.h
class NoteWidget;  // Forward declaration
class SettingsPage;  // Forward declaration if needed

class Qwote : public QWidget
{
    Q_OBJECT
public:
    explicit Qwote(QWidget *parent = nullptr);
    ~Qwote();
    void createNewNote();

private slots:
    void onStartupActionStateChanged();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onSettingsPageClosed();
    void quitQwote();

private:
    void createTrayIcon();
    QSystemTrayIcon *trayIcon;
    QAction *startupAction;
    bool restoreSavedNotes();
    QList<NoteWidget*> noteWidgets;  // This can use the forward declaration
    SettingsPage* settingsPage;
    void showSettings();
    void deleteAllNotes();

signals:
};

#endif // QWOTE_H
