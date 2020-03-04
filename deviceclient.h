#ifndef DEVICECLIENT_H
#define DEVICECLIENT_H

#include "QObject"
#include "qdebug.h"


class deviceClient : public QObject
{
    Q_OBJECT
public:
    explicit deviceClient(QObject *parent = nullptr);



signals:
    void writeData(QString);
    void sendDataToMacs(QStringList addresses,QString data);
    void sendDataToFunction(QString function,QString data);
    void vbattRead(double vbat);
public slots:
private slots:
    void dataReceived(QString);
};

#endif // DEVICECLIENT_H
