#ifndef QWOTESERVER_H
#define QWOTESERVER_H

#include <QLocalServer>
#include <QLocalSocket>
#include "qwote.h"  // Assuming Qwote is defined in qwote.h

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
    Qwote qwote; // Your Qwote instance
};

#endif // QWOTESERVER_H
