#ifndef CONTROLLERCLIENT_H
#define CONTROLLERCLIENT_H

#include <QObject>
#include "qdebug.h"



class controllerClient : public QObject
{
    Q_OBJECT
public:
    explicit controllerClient(QObject *parent = nullptr);
    ~controllerClient(void);

signals:
    void writeData(QString);

private slots:
    void dataReceived(QString data);

public slots:
};

#endif // CONTROLLERCLIENT_H