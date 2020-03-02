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
#include "tcpclient.h"
#include "qthread.h"

class tcpSocketServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit tcpSocketServer(QObject *parent = nullptr, QString ip="127.0.0.1", qint64 port=1000);

private:
    std::vector<tcpClient*> clients;
private slots:
    void broadcastData(QString data);
    void testRead(void);
    void ping(void);
    void incomingDisconnection(void);
    void threadDestroyed(void);
protected:
    void incomingConnection(qintptr socketDescriptor) override;


public slots:
};

#endif // TCPSOCKETSERVER_H
