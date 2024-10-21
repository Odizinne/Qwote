#include <qwote.h>
#include <QApplication>
#include <QSharedMemory>
#include <QDebug>
#include "QwoteServer.h"

int main(int argc, char *argv[]) {
    qDebug() << "Starting application";
    QSharedMemory sharedMemory("QwoteID");

    // Check if another instance is running
    if (sharedMemory.attach()) {
        qDebug() << "Attempting to connect to existing instance";
        QLocalSocket socket;
        socket.connectToServer("QwoteServer");

        // Check if the connection was successful
        if (socket.waitForConnected(3000)) { // Wait for 3 seconds for connection
            QByteArray block;
            QDataStream out(&block, QIODevice::WriteOnly);
            out << QString("createNewNote");
            qDebug() << "Sending request to create a new note";
            socket.write(block);
            socket.flush();
            socket.waitForBytesWritten();
            socket.disconnectFromServer();
            qDebug() << "Request sent successfully";
        } else {
            qDebug() << "Failed to connect to server:" << socket.errorString();
        }
        return 0; // Exit if another instance is running
    }

    // Create shared memory to ensure single instance
    if (!sharedMemory.create(1)) {
        qDebug() << "Failed to create shared memory:" << sharedMemory.errorString();
        return 1;
    }

    QApplication app(argc, argv);
    app.setStyle("fusion");
    app.setQuitOnLastWindowClosed(false);

    Qwote qwote;

    try {
        QwoteServer server(&qwote);
    } catch (const std::runtime_error &e) {
        qDebug() << "Failed to start QwoteServer:" << e.what();
        return 1;
    }

    return app.exec(); // Start the application event loop
}
