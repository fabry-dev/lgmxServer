#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include "QTcpServer"
#include "QTcpSocket"
#include "QThread"
#include "QDebug"
#include "controllerclient.h"
#include "deviceclient.h"
#include "qhostinfo.h"
#include "qprocess.h"



class tcpClient;

class tcpClientControl : public QObject
{
    Q_OBJECT
public:
    explicit tcpClientControl(QObject *parent = nullptr, int socketDescriptor=0, QString PATH="");
    ~tcpClientControl(void);
    void sendData(QString data){emit shouldSendData(data);}
    QString getMacAddress(void){return macAddress;}
    QString getFunction(void){return function;}
    double getVbat(void){return vbat;}

private:
    QString PATH;
    tcpClient *client;
    QString macAddress;
    QString function;
    double vbat;
signals:
    void disconnected();
    void shouldSendData(QString);
    void requestDevicesList(void);
    void disconnectFromHost(void);
    void solvedMacAddress(QString);

private slots:
    void setMacAddress(QString nuMac){macAddress = nuMac;}
    void setFunction(QString nuFunction){function = nuFunction;}
    void setVbat(double nuVbat){vbat = nuVbat;}

};



class tcpClient : public QObject
{
    Q_OBJECT
public:
    explicit tcpClient(QObject *parent = nullptr, int socketDescriptor=0, QString PATH="");
    QString getMacAddress() const;

private:
    int socketDescriptor;
    QString PATH;
    QString macAddress;
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
    void vbatRead(double);
    void sendDataToMacs(QStringList,QString);
    void sendDataToFunction(QString,QString);
    void requestDevicesList(void);
    void reloadPresets(void);
    void sendPresetsList(QString function,QStringList addresses);
    void loadPreset(QString preset);
private slots:
    void readData(void);
    void writeData(QString data);
    void disconnectFromHost(void);
    void run() ;
};







#endif // TCPCLIENT_H
