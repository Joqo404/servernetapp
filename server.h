#ifndef SERVER_H
#define SERVER_H
#include <QTcpSocket>
#include <QTcpServer>
#include <QVector>

class server : public QTcpServer
{
    Q_OBJECT

public:
    server();

private:
    QTcpSocket *serv_sock;
    QVector <QTcpSocket*> server_datasocket;
    QByteArray data;
    void sendToClient(QString str);
public slots:
    void slotReadyRead();
    void incomingConnection(qintptr socketDescriptor);

};

#endif // SERVER_H
