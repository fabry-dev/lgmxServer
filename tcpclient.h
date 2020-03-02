#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include "QTcpServer"
#include "QTcpSocket"
#include "QThread"
#include "QDebug"
#include "QDataStream"
#include "controllerclient.h"
#include "deviceclient.h"
#include "qhostinfo.h"
#include "qprocess.h"
class tcpClient : public QObject
{
    Q_OBJECT
public:
    explicit tcpClient(QObject *parent = nullptr,int socketDescriptor=0);
    ~tcpClient(void);
    QString getMacAddress(void);
private:
    int socketDescriptor;
    QTcpSocket *tcpSocket;
    QObject *clientFunction;
    void makeClientDevice(void);
    void makeClientController(void);
    void handleData(QString data);
    QString macAddress;
    QString resolveMacAddress(bool *success);
signals:
    void error(QTcpSocket::SocketError socketError);
    void disconnected(void);
    void dataReceived(QString);
private slots:
    void readData(void);
    void writeData(QString data);
    void run() ;
};







#endif // TCPCLIENT_H
