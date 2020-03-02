#include "deviceclient.h"

deviceClient::deviceClient(QObject *parent) : QObject(parent)
{
    connect(parent,SIGNAL(dataReceived(QString)),this,SLOT(dataReceived(QString)));
    connect(this,SIGNAL(writeData(QString)),parent,SLOT(writeData(QString)));
    qDebug()<<"client is a device";
    emit writeData("device=YES");
}



deviceClient::~deviceClient()
{


//qDebug()<<"device is destroyed";
}




void deviceClient::dataReceived(QString data)
{

qDebug()<<"device <<"<<data;

}


