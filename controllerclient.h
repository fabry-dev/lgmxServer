#ifndef CONTROLLERCLIENT_H
#define CONTROLLERCLIENT_H

#include <QObject>
#include "qdebug.h"
#include "tcpclient.h"
#include "qfile.h"

class controllerClient : public QObject
{
    Q_OBJECT
public:
    explicit controllerClient(QObject *parent = nullptr, QString PATH="");
    ~controllerClient(void);
private:
    QString PATH;
signals:
    void writeData(QString);
    void sendDataToMacs(QStringList addresses,QString data);
    void sendDataToFunction(QString function,QString data);


private slots:
    void dataReceived(QString data);

public slots:
};

#endif // CONTROLLERCLIENT_H
