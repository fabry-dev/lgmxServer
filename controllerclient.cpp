#include "controllerclient.h"

controllerClient::controllerClient(QObject *parent) : QObject(parent)
{
    connect(parent,SIGNAL(dataReceived(QString)),this,SLOT(dataReceived(QString)));
    connect(this,SIGNAL(writeData(QString)),parent,SLOT(writeData(QString)));
    connect(this,SIGNAL(sendDataToMacs(QStringList,QString)),parent,SIGNAL(sendDataToMacs(QStringList,QString)));
    connect(this,SIGNAL(sendDataToFunction(QString,QString)),parent,SIGNAL(sendDataToFunction(QString,QString)));

    qDebug()<<"client is a controller";
    emit writeData("controller=YES");
}


controllerClient::~controllerClient()
{


    //qDebug()<<"controller is destroyed";
}




void controllerClient::dataReceived(QString data)
{
    QStringList addresses = {};
    QString payload = "";
    QString function = "";

    qDebug()<<"controller <<"<<data;



    QStringList fields = data.split("|",QString::SkipEmptyParts);
    for(QString field:fields)
    {
        if(field == "getdevices")
        {
            emit ((tcpClient*)parent())->requestDevicesList();
            continue;
        }

        QStringList values = field.split("=",QString::SkipEmptyParts);
        if(values.size()==2)
        {
            QString paramName = values[0];
            QString paramValue = values[1];
            if(paramName == "target")//this parameter defines to which mac adresses the data will be forwarded
                addresses = paramValue.split(",",QString::SkipEmptyParts);
            else if(paramName == "functiontarget")//the parameter defubes to which client function the data will be forwarded
                function = paramValue;
            else if(paramName == "msg")
                payload = paramValue;
        }
    }

    if(payload.size()>0)
    {
        if(addresses.size()>0)
            emit sendDataToMacs(addresses,payload);
        if(function.size()>0)
        {
            emit sendDataToFunction(function,payload);
        }
    }
}
