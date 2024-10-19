#include <QApplication>
#include <qwote.h>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setStyle("fusion");
    app.setQuitOnLastWindowClosed(false);
    Qwote w;
    return app.exec();
}
