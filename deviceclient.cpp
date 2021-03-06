#include "deviceclient.h"

deviceClient::deviceClient(QObject *parent) : QObject(parent)
{

    connect(parent,SIGNAL(dataReceived(QString)),this,SLOT(dataReceived(QString)));
    connect(this,SIGNAL(writeData(QString)),parent,SLOT(writeData(QString)));
    connect(this,SIGNAL(sendDataToMacs(QStringList,QString)),parent,SIGNAL(sendDataToMacs(QStringList,QString)));
    connect(this,SIGNAL(sendDataToFunction(QString,QString)),parent,SIGNAL(sendDataToFunction(QString,QString)));
    connect(this,SIGNAL(vbattRead(double)),parent,SIGNAL(vbatRead(double)));
    qDebug()<<"client is a device";
    emit writeData("device=YES");
}



void deviceClient::dataReceived(QString data)
{

   // qDebug()<<"device <<"<<data;
    qDebug()<<"device:"<<((tcpClient*)parent())->getMacAddress() << ">>"<<data;


    QStringList addresses = {};
    QString payload = "";
    QString function = "";


    QStringList fields = data.split("|",QString::SkipEmptyParts);

    for(QString field:fields)
    {
        QStringList values = field.split("=",QString::SkipEmptyParts);
        if(values.size()==2)
        {
            QString paramName = values[0];
            QString paramValue = values[1];
            if(paramName == "target")//this parameter defines to which mac adresses the data will be forwarded
                addresses = paramValue.split(",",QString::SkipEmptyParts);
            else if(paramName == "functiontarget")//the parameter defines to which client function the data will be forwarded
                function = paramValue;
            else if(paramName == "msg")
                payload = paramValue;
            else if(paramName == "vbat")
            {
                double vbat;bool test;
                vbat = paramValue.toDouble(&test);
                if((test)&&(vbat>0))
                    emit vbattRead(vbat);
            }

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


