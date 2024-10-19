#ifndef QWOTE_H
#define QWOTE_H

#include <QWidget>
#include <QSystemTrayIcon>
#include "notewidget.h"

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

private:
    void createTrayIcon();
    QSystemTrayIcon *trayIcon;
    QAction *startupAction;
    bool restoreSavedNotes();
    QList<NoteWidget*> noteWidgets;

signals:
};

#endif // QWOTE_H
