#ifndef TCPSOCKETSERVER_H
#define TCPSOCKETSERVER_H

#include <QObject>
#include "qtcpserver.h"
#include "qtcpsocket.h"
#include "qnetworkinterface.h"
#include "qtimer.h"
#include "QDebug"
#include "qdatetime.h"
#include "QDataStream"

class tcpSocketServer : public QObject
{
    Q_OBJECT
public:
    explicit tcpSocketServer(QObject *parent = nullptr, QString ip="192.168.4.1",QString port="6000");

private:
    QTcpServer *tcpServer;
    std::vector<QTcpSocket*> clients;
private slots:
    void gotNewConnection(void);
    void gotNewDisconnection(void);
    void broadcastData(QString data);

public slots:
};

#endif // TCPSOCKETSERVER_H
