#include "QwoteServer.h"

QwoteServer::QwoteServer(QObject *parent)
    : QObject(parent), server(new QLocalServer(this)) {}

bool QwoteServer::startServer(const QString &serverName)
{
    if (!server->listen(serverName)) {
        return false;
    }

    connect(server, &QLocalServer::newConnection, this, &QwoteServer::handleNewConnection);
    return true;
}

void QwoteServer::handleNewConnection()
{
    QLocalSocket *clientConnection = server->nextPendingConnection();

    connect(clientConnection, &QLocalSocket::readyRead, this, &QwoteServer::handleClientData);
}

void QwoteServer::handleClientData()
{
    QLocalSocket *clientConnection = qobject_cast<QLocalSocket *>(sender());
    QDataStream in(clientConnection);
    QString command;
    in >> command;

    if (command == "createNewNote") {
        qwote.createNewNote();
    }

    clientConnection->disconnectFromServer();
    clientConnection->deleteLater();
}

void QwoteServer::stopServer()
{
    if (server->isListening()) {
        server->close();
    }
}
