#include "server.h"

server::server()
{
    if(this->listen(QHostAddress::Any, 5555))
    {
        qDebug() << "serv is start";
    }
    else
    {
        qDebug() << "error";
    }
}

void server::incomingConnection(qintptr socketDescriptor)
{
    serv_sock = new QTcpSocket;
    serv_sock->setSocketDescriptor(socketDescriptor);
    connect(serv_sock, &QTcpSocket::readyRead, this, &server::slotReadyRead);
    connect(serv_sock, &QTcpSocket::disconnected, serv_sock, &QTcpSocket::deleteLater);
    server_datasocket.push_back(serv_sock);
    qDebug()<< "client connect" << socketDescriptor;
}

void server::slotReadyRead(){
    serv_sock = (QTcpSocket *)sender();
    QDataStream in(serv_sock);
    in.setVersion(QDataStream::Qt_6_4);

    if (in.status() == QDataStream::Ok)
    {
        qDebug()<< "okey";
        QString str;
        in >> str;
        qDebug() << str;
    }
    else
    {
        qDebug() << "error";
    }
}

void server::sendToClient(QString str){
    data.clear();
    QDataStream out (&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    out << str;
    serv_sock->write(data);
}
