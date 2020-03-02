#ifndef DEVICECLIENT_H
#define DEVICECLIENT_H

#include "QObject"
#include "qdebug.h"


class deviceClient : public QObject
{
    Q_OBJECT
public:
    explicit deviceClient(QObject *parent = nullptr);
    ~deviceClient();

signals:
    void writeData(QString);

public slots:
private slots:
    void dataReceived(QString);
};

#endif // DEVICECLIENT_H
