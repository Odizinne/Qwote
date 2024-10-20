#ifndef QWOTESERVER_H
#define QWOTESERVER_H

#include <QLocalServer>
#include <QLocalSocket>
#include "qwote.h"

class QwoteServer : public QObject {
    Q_OBJECT

public:
    QwoteServer(QObject *parent = nullptr);
    bool startServer(const QString &serverName);

private slots:
    void handleNewConnection();
    void handleClientData();

private:
    QLocalServer *server;
    Qwote qwote;
};

#endif // QWOTESERVER_H
