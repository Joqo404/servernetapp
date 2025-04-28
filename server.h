#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QUdpSocket>

class server : public QTcpServer
{
    Q_OBJECT
public:
    server();

private slots:
    void slotReadyRead();
    void processPendingDatagrams();

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
    QList<QTcpSocket*> clients;
    QMap<QTcpSocket*, QString> authorizedClients;
    QMap<QTcpSocket*, quint16> clientBlockSizes;

    QUdpSocket *udpSocket;

    bool validateCredentials(const QString &login, const QString &passwordHash);
    void sendToClient(QTcpSocket *socket, const QString &message);
};
