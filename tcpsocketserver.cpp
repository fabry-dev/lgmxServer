#include "tcpsocketserver.h"


tcpSocketServer::tcpSocketServer(QObject *parent,QString ip,qint64 port,QString PATH) : QTcpServer(parent),PATH(PATH)
{

    defautState = "";
    reloadPresets();

    while(!listen(QHostAddress(ip),port))
    {

        qDebug()<<"tcp server not connected, retrying in 5s";
        QThread::sleep(5);
    }

    qDebug()<<"tcp server running on "<<serverAddress().toString()<<":"<<serverPort();



}

void tcpSocketServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug()<<"connection";

    tcpClientControl *client = new tcpClientControl(this,socketDescriptor,PATH);
    connect(client,SIGNAL(disconnected()),this,SLOT(incomingDisconnection()));
    connect(client,SIGNAL(requestDevicesList()),this,SLOT(sendDevicesDescriptionToMacs()));
    clients.push_back(client);//save client in the clients pool



    qDebug()<<clients.size()<<" devices connected";



}

void tcpSocketServer::incomingDisconnection()
{
    tcpClientControl * client = (tcpClientControl*)QObject::sender();
    clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
    qDebug()<<clients.size()<<" devices connected";
}


void tcpSocketServer::newMacRegistered(void)
{
    tcpClientControl * nuClient = (tcpClientControl*)QObject::sender();

    for(auto client:clients)
    {
        if(client == nuClient)//ignore the new client
            continue;



        if((client->getMacAddress()!="")&&(client->getMacAddress()==nuClient->getMacAddress()))
        {
            qDebug()<<"mac address"<<client->getMacAddress()<<"was previously registered, disconnecting now.";
            emit client->disconnectFromHost();
        }

    }

    QString msg = loadMacMessage(nuClient->getMacAddress());
    if(msg!="")
        sendDataToMacs(QStringList()<<nuClient->getMacAddress(),msg);



}




//send data to all clients which mac address is macAddresses
void tcpSocketServer::sendDataToMacs(QStringList macAddresses,QString data)
{
    bool success = false;
    for(auto client:clients)
    {
        if(macAddresses.contains(client->getMacAddress()))
        {
            success = true;
            qDebug()<<client->getFunction()<<":"<<client->getMacAddress()<<"<<"<<data;
            client->sendData(data);
            if(client->getFunction() == "device")
                storeMacMessage(client->getMacAddress(),data);//store the specific message
        }
    }

    if(!success)
        qDebug()<<"no mac address found";

}

//send data to any client which function is "function"
void tcpSocketServer::sendDataToFunction(QString function,QString data)
{

    for(auto client:clients)
    {
        if(client->getFunction() == function)
        {
            qDebug()<<client->getFunction()<<":"<<client->getMacAddress()<<"<<"<<data;
            if(function=="device")
            {
                defautState = data;//store the default message
                storeMacMessage(client->getMacAddress(),data);//store the specific message
            }

            client->sendData(data);
        }
    }


}


//send device infos (mac lists and battery voltage) to the requesting controller
void tcpSocketServer::sendDevicesDescriptionToMacs(void)
{
    QString msg;
    QStringList macAddresses;
    tcpClientControl *client = (tcpClientControl*)QObject::sender();
    macAddresses.append(client->getMacAddress());
    msg+="{";
    for(auto device:clients)
    {
        if(device->getFunction()!="device")
            continue;

        if(msg.size()>1)
            msg+=",";

        QString data = "{mac="+device->getMacAddress()+",vbat="+QString::number(device->getVbat())+"}";
        msg+=data;
    }
    msg+="}";

    sendDataToMacs(macAddresses,msg);
}

//reload the presets from the cfg files and store them in "presetList"
void tcpSocketServer::reloadPresets(void)
{
    QDir directory(PATH+"presets");
    QStringList presets = directory.entryList(QStringList() << "*.cfg",QDir::Files);
    presetList.clear();


    for(auto preset:presets)
    {
        struct preset_t nuPreset;
        nuPreset.name = (QString)preset.mid(0,preset.length()-4);

        QFile file(PATH+"presets/"+preset);
        if(!file.open(QIODevice::ReadOnly)) {
            qDebug()<<"no"<<preset<<"file";
        }
        else
        {
            QTextStream in(&file);
            nuPreset.content = in.readAll();
            file.close();
            presetList.push_back(nuPreset);
        }
    }

    qDebug()<<"preset list:";
    for(auto preset:presetList)
    {
        qDebug()<<preset.name<<preset.content;
    }


}


//provide a list of the available presets
void tcpSocketServer::sendPresetList(QString function,QStringList addresses)
{
    QString list;
    for(auto preset:presetList)
    {
        list+=preset.name;
        list+="|";
    }
    if(list.size()>0)
        list = list.mid(0,list.length()-1);


    if(function.size()>0)
        sendDataToFunction(function,list);
    if(addresses.size()>0)
        sendDataToMacs(addresses,list);

}


//decode a preset
void tcpSocketServer::loadPreset(QString presetName)
{

    struct preset_t preset;
    bool found = false;
    for(auto preset2:presetList)
    {
        if(preset2.name==presetName)
        {
            found = true;
            preset = preset2;

            break;
        }
    }

    if(found)
    {
        QStringList addresses;
        QString functionTarget;
        QString payload;
        QStringList fields = preset.content.split("|",QString::SkipEmptyParts);
        for(QString field:fields)
        {
            QStringList values = field.split("=",QString::SkipEmptyParts);
            if(values.size()==2)
            {
                QString paramName = values[0];
                QString paramValue = values[1];
                if(paramName == "target")//this parameter defines to which mac adresses the data will be forwarded
                    addresses = paramValue.split(",",QString::SkipEmptyParts);
                else if(paramName == "functiontarget")//the parameter defubes to which client function the data will be forwarded
                    functionTarget = paramValue;
                else if(paramName == "msg")
                    payload = paramValue;
            }
        }

        if(payload.size()>0)
        {
            if(addresses.size()>0)
                sendDataToMacs(addresses,payload);
            if(functionTarget.size()>0)
            {
                sendDataToFunction(functionTarget,payload);
            }
        }

    }
    else
        qDebug()<<"preset "<<presetName<<"not found";


}

//store the last message sent to a mac address so that we cann resend it upon reconnection
void tcpSocketServer::storeMacMessage(QString mac,QString msg)
{
    for(int i =0;i<stateList.size();i++)
    {
        if(stateList[i].mac==mac)
        {
            stateList[i].msg = msg;
            return;
        }
    }

    //if we reached this par the mac was not found, we need to create a new entry
    struct state_t nuState;
    nuState.mac = mac;
    nuState.msg = msg;
    stateList.push_back(nuState);
}

//load the last message sent to this mac address
QString tcpSocketServer::loadMacMessage(QString mac)
{

    for(auto state:stateList)
    {
        if(state.mac==mac)
            return state.msg;
    }
    //if we reached this, let's revert to default
    return defautState;
}
