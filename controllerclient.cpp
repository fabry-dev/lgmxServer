#include "controllerclient.h"

controllerClient::controllerClient(QObject *parent) : QObject(parent)
{
    connect(parent,SIGNAL(dataReceived(QString)),this,SLOT(dataReceived(QString)));
    connect(this,SIGNAL(writeData(QString)),parent,SLOT(writeData(QString)));
    qDebug()<<"client is a controller";
    emit writeData("controller=YES");
}


controllerClient::~controllerClient()
{


//qDebug()<<"controller is destroyed";
}




void controllerClient::dataReceived(QString data)
{

qDebug()<<"controller <<"<<data;

}
