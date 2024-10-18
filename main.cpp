#include <QApplication>
#include "NoteWidget.h"
#include <qwote.h>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    //app.setStyle("fusion");
    app.setQuitOnLastWindowClosed(false);
    //NoteWidget::createNewNote();
    Qwote w;
    return app.exec();
}
