#include <QApplication>
#include <QLocalServer>
#include <QLocalSocket>
#include <QDebug>
#include <QLocale>
#include <QTranslator>
#include <Qwote.h>

const QString SERVER_NAME = "QwoteServer";

int main(int argc, char *argv[]) {
#ifdef __linux__
    qputenv("QT_QPA_PLATFORM", "xcb");
#endif

    QApplication app(argc, argv);

    QLocalSocket socket;
    socket.connectToServer(SERVER_NAME);

    if (socket.waitForConnected(100)) {
        qDebug() << "Another instance is already running. Exiting...";
        return 0;
    }

    QLocalServer server;
    if (!server.listen(SERVER_NAME)) {
        qDebug() << "Unable to start the server:" << server.errorString();
        return 1;
    }

    QObject::connect(&server, &QLocalServer::newConnection, [&server]() {
        QLocalSocket *clientConnection = server.nextPendingConnection();
        clientConnection->disconnectFromServer();
    });

    QLocale locale;
    QString languageCode = locale.name().section('_', 0, 0);
    QTranslator translator;
    if (translator.load(":/translations/tr/Qwote_" + languageCode + ".qm")) {
        app.installTranslator(&translator);
    }

    app.setStyle("fusion");
    app.setQuitOnLastWindowClosed(false);
    Qwote qwote;

    return app.exec();
}
