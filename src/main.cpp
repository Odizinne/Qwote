#include <QApplication>
#include <QSharedMemory>
#include "qwoteserver.h"

const QString SERVER_NAME = "QwoteServer";

int main(int argc, char *argv[]) {
    QSharedMemory sharedMemory("QwoteID");

    if (sharedMemory.attach()) {
        QLocalSocket socket;
        socket.connectToServer(SERVER_NAME);

        if (socket.waitForConnected()) {
            QByteArray block;
            QDataStream out(&block, QIODevice::WriteOnly);
            out << QString("createNewNote");
            socket.write(block);
            socket.flush();
            socket.waitForBytesWritten();
            socket.disconnectFromServer();
        }
        return 0;
    }

    if (!sharedMemory.create(1)) {
        return 1;
    }

    QApplication app(argc, argv);
    app.setStyle("fusion");
    app.setQuitOnLastWindowClosed(false);

    QwoteServer qwoteServer;
    if (!qwoteServer.startServer(SERVER_NAME)) {
        return 1;
    }

    return app.exec();
}
