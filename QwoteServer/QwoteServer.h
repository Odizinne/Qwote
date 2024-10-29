#ifndef QWOTESERVER_H
#define QWOTESERVER_H

#include <QLocalServer>
#include <QLocalSocket>
#include "Qwote.h"

class QwoteServer : public QObject {
    Q_OBJECT

public:
    QwoteServer(QObject *parent = nullptr);
    bool startServer(const QString &serverName);
    void stopServer();

private slots:
    void handleNewConnection();
    void handleClientData();

private:
    QLocalServer *server;
    Qwote qwote;
};

#endif // QWOTESERVER_H
