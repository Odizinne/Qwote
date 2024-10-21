#ifndef QWOTESERVER_H
#define QWOTESERVER_H

#include <QObject>
#include <QLocalServer>
#include <QLocalSocket>
#include <QDataStream>
#include <qwote.h>

class QwoteServer : public QObject {
    Q_OBJECT
public:
    explicit QwoteServer(Qwote *qwote, QObject *parent = nullptr);

private slots:
    void onNewConnection();
    void onReadyRead();

private:
    QLocalServer server;
    Qwote *qwote;
};

#endif // QWOTESERVER_H
