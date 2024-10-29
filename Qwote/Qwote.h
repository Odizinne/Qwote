#ifndef QWOTE_H
#define QWOTE_H

#include <QWidget>
#include <QSystemTrayIcon>
#include "SettingsPage.h"

class NoteWidget;

class Qwote : public QWidget
{
    Q_OBJECT
public:
    explicit Qwote(QWidget *parent = nullptr);
    ~Qwote();
    void createNewNote();
    void onNoteDeleted(NoteWidget *noteWidget);

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
    QList<NoteWidget*> noteWidgets;
    SettingsPage* settingsPage;
    void showSettings();
    void deleteAllNotes();

};

#endif // QWOTE_H
