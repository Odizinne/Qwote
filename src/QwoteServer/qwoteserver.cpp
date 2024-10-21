#include "QwoteServer.h"

QwoteServer::QwoteServer(Qwote *qwote, QObject *parent)
    : QObject(parent), qwote(qwote) {
    if (!server.listen("QwoteServer")) {
        throw std::runtime_error("Failed to start QwoteServer");
    }

    connect(&server, &QLocalServer::newConnection, this, &QwoteServer::onNewConnection);
}

void QwoteServer::onNewConnection() {
    QLocalSocket *clientConnection = server.nextPendingConnection();

    connect(clientConnection, &QLocalSocket::readyRead, this, &QwoteServer::onReadyRead);
}

void QwoteServer::onReadyRead() {
    QLocalSocket *clientConnection = qobject_cast<QLocalSocket*>(sender());
    if (!clientConnection) {
        return;
    }

    QDataStream in(clientConnection);
    QString command;
    in >> command;

    if (command == "createNewNote") {
        qwote->createNewNote();
    }

    clientConnection->disconnectFromServer();
    clientConnection->deleteLater(); // Safely delete the connection
}
