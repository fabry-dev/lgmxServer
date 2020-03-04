#include "controllerclient.h"

controllerClient::controllerClient(QObject *parent,QString PATH) : QObject(parent),PATH(PATH)
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

    //qDebug()<<"controller <<"<<data;
    qDebug()<<"controller:"<<((tcpClient*)parent())->getMacAddress() << ">>"<<data;


    QStringList fields = data.split("|",QString::SkipEmptyParts);
    for(QString field:fields)
    {
        if(field == "getdevices")
        {
            emit ((tcpClient*)parent())->requestDevicesList();
            break;
        }

        if(field == "listpresets")
        {
            emit ((tcpClient*)parent())->sendPresetsList("",QStringList()<<((tcpClient*)parent())->getMacAddress());
            break;
        }


        if((field == "savepreset")&&(fields.size()>=3))
        {
            QString presetName = fields[1];


            //let's find the 2nd "|" position which marks the beginning of the content
            int contentStartPos ;
            int count = 0;
            for(contentStartPos = 0;contentStartPos<data.size();contentStartPos++)
            {
                if(data.at(contentStartPos)=="|")
                    count++;
                if(count >= 2)
                    break;
            }

            if(contentStartPos+1>=data.size())
                break;

            QString presetContent = data.mid(contentStartPos+1);
            QString filename = PATH+"presets/"+presetName+".cfg";
            QFile file(filename);
            if(!file.open(QFile::WriteOnly|QFile::Truncate))
            {
                qDebug()<<"cannot open file"<<filename;

            }
            else
            {
                QTextStream stream( &file );
                stream << presetContent;
                file.close();
                qDebug()<<"preset saved";
                emit ((tcpClient*)parent())->reloadPresets();
            }

            break;
        }


        if((field == "loadpreset")&&(fields.size()==2))
        {
            emit ((tcpClient*)parent())->loadPreset(fields[1]);
            break;
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
