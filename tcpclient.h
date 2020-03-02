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



class tcpClient;

class tcpClientControl : public QObject
{
    Q_OBJECT
public:
    explicit tcpClientControl(QObject *parent = nullptr,int socketDescriptor=0);
    void sendData(QString data){emit shouldSendData(data);}
    QString getMacAddress(void){return macAddress;}
    QString getFunction(void){return function;}
private:
    tcpClient *client;
    QString macAddress;
    QString function;
signals:
    void disconnected();
    void shouldSendData(QString);
    void sendDataToMacs(QStringList,QString);
    void sendDataToFunction(QString,QString);
private slots:
    void setMacAddress(QString nuMac){macAddress = nuMac;}
    void setFunction(QString nuFunction){function = nuFunction;}

};



class tcpClient : public QObject
{
    Q_OBJECT
public:
    explicit tcpClient(QObject *parent = nullptr,int socketDescriptor=0);

private:
    int socketDescriptor;
    QTcpSocket *tcpSocket;
    QObject *clientFunction;
    void makeClientDevice(void);
    void makeClientController(void);
    void handleData(QString data);
    QString resolveMacAddress(bool *success);
signals:
    void error(QTcpSocket::SocketError socketError);
    void disconnected(void);
    void dataReceived(QString);
    void solvedMacAddress(QString);
    void functionChosen(QString);
    void sendDataToMacs(QStringList,QString);
    void sendDataToFunction(QString,QString);
private slots:
    void readData(void);
    void writeData(QString data);
    void run() ;
};







#endif // TCPCLIENT_H
