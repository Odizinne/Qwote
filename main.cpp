#include <QApplication>
#include <QSharedMemory>
#include <QLocalSocket>
#include "QwoteServer.h"
#include <QLocale>
#include <QTranslator>

const QString SERVER_NAME = "QwoteServer";

int main(int argc, char *argv[]) {
#ifdef __linux__
    qputenv("QT_QPA_PLATFORM", "xcb");
#endif
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

    QLocale locale;
    QString languageCode = locale.name().section('_', 0, 0);
    QTranslator translator;
    if (translator.load(":/translations/tr/Qwote_" + languageCode + ".qm")) {
        app.installTranslator(&translator);
    }

    app.setStyle("fusion");
    app.setQuitOnLastWindowClosed(false);

    QwoteServer qwoteServer;
    if (!qwoteServer.startServer(SERVER_NAME)) {
        return 1;
    }

    QObject::connect(&app, &QApplication::aboutToQuit, [&]() {
        qwoteServer.stopServer();
        sharedMemory.detach();
    });

    return app.exec();
}
