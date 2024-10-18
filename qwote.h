#ifndef QWOTE_H
#define QWOTE_H

#include <QWidget>
#include <QSystemTrayIcon>

class Qwote : public QWidget
{
    Q_OBJECT
public:
    explicit Qwote(QWidget *parent = nullptr);

private:
    void createTrayIcon();
    QSystemTrayIcon *trayIcon;
signals:
};

#endif // QWOTE_H
