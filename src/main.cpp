#include <qwote.h>
#include <QApplication>
#include <QSharedMemory>
#include <QLocalServer>
#include <QLocalSocket>
#include <QDataStream>

const QString SERVER_NAME = "QwoteServer";

int main(int argc, char *argv[]) {
    QSharedMemory sharedMemory("QwoteID");

    if (sharedMemory.attach()) {
        // If the shared memory is already attached, try to connect to the existing instance
        QLocalSocket socket;
        socket.connectToServer(SERVER_NAME);

        if (socket.waitForConnected()) {
            // Send a command to create a new note
            QByteArray block;
            QDataStream out(&block, QIODevice::WriteOnly);
            out << QString("createNewNote");
            socket.write(block);
            socket.flush();
            socket.waitForBytesWritten();
            socket.disconnectFromServer();
        }
        return 0; // Exit the second instance
    }

    if (!sharedMemory.create(1)) {
        return 1; // Failed to create shared memory
    }

    QApplication app(argc, argv);
    app.setStyle("fusion");
    app.setQuitOnLastWindowClosed(false);

    // Create a local server to listen for incoming connections
    QLocalServer server;
    if (!server.listen(SERVER_NAME)) {
        return 1; // Failed to create the local server
    }

    // Create the Qwote instance
    Qwote qwote;

    // Handle incoming connections in the main event loop
    QObject::connect(&server, &QLocalServer::newConnection, [&qwote, &server]() {
        QLocalSocket *clientConnection = server.nextPendingConnection();

        // Handle the data from the client
        QObject::connect(clientConnection, &QLocalSocket::readyRead, [clientConnection, &qwote]() {
            QDataStream in(clientConnection);
            QString command;
            in >> command;

            if (command == "createNewNote") {
                // Call the createNewNote function of the first instance
                qwote.createNewNote();
            }

            clientConnection->disconnectFromServer();
        });
    });

    return app.exec();
}
