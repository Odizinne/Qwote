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

private slots:
    void onStartupActionStateChanged();

private:
    void createTrayIcon();
    QSystemTrayIcon *trayIcon;
    QAction *startupAction;
    bool restoreSavedNotes();
    void createNewNote();
    QList<NoteWidget*> noteWidgets;

signals:
};

#endif // QWOTE_H
