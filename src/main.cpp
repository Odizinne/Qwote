#include <qwote.h>
#include <QApplication>
#include <QSharedMemory>

int main(int argc, char *argv[]) {
    QSharedMemory sharedMemory("QwoteID");

    if (sharedMemory.attach()) {
        return 0;
    }

    if (!sharedMemory.create(1)) {
        return 1;
    }

    QApplication app(argc, argv);
    app.setStyle("fusion");
    app.setQuitOnLastWindowClosed(false);
    Qwote qwote;
    return app.exec();
}
