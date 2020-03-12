#ifndef TCPSOCKETSERVER_H
#define TCPSOCKETSERVER_H

#include <QObject>
#include "qtcpserver.h"
#include "qtcpsocket.h"
#include "qnetworkinterface.h"
#include "qtimer.h"
#include "QDebug"
#include "qdatetime.h"
#include "tcpclient.h"
#include "qthread.h"
#include "qfile.h"
#include "qdir.h"

struct preset_t
{
    QString name;
    QString content;
};

class tcpSocketServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit tcpSocketServer(QObject *parent = nullptr, QString ip="127.0.0.1", qint64 port=1000,QString PATH="");

private:
    QString PATH;
    std::vector<tcpClientControl*> clients;
    std::vector<preset_t> presetList;

private slots:
    void incomingDisconnection(void);
    void sendDataToMac(QString macAddress, QString data);
    void sendDataToMacs(QStringList macAddresses,QString data);
    void sendDataToFunction(QString function,QString data);
    void sendDevicesDescriptionToMacs(void);
    void reloadPresets(void);
    void loadPreset(QString presetName);
    void sendPresetList(QString function,QStringList addresses);
    void newMacRegistered(void);
protected:
    void incomingConnection(qintptr socketDescriptor) override;


public slots:
};

#endif // TCPSOCKETSERVER_H
