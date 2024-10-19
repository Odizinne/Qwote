#ifndef QWOTE_H
#define QWOTE_H

#include <QWidget>
#include <QSystemTrayIcon>

class Qwote : public QWidget
{
    Q_OBJECT
public:
    explicit Qwote(QWidget *parent = nullptr);

private slots:
    void onStartupActionStateChanged();

private:
    void createTrayIcon();
    QSystemTrayIcon *trayIcon;
    QAction *startupAction;
    bool restoreSavedNotes();
    void createNewNote();

signals:
};

#endif // QWOTE_H
