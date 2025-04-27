#ifndef SERVER_H
#define SERVER_H

#include <QTcpSocket>
#include <QTcpServer>
#include <QMap>
#include <QList>
#include <QCryptographicHash>
#include <QDebug>

class server : public QTcpServer
{
    Q_OBJECT

public:
    server();

protected:
    void incomingConnection(qintptr socketDescriptor) override; // Обработка нового подключения

private slots:
    void slotReadyRead(); // Слот для обработки входящих данных

private:
    QByteArray data;

    QList<QTcpSocket*> clients; // Список всех подключённых клиентов
    QMap<QTcpSocket*, QString> authorizedClients; // Сопоставление сокета с логином

    QMap<QString, QString> usersDatabase; // База данных пользователей (логин -> хэш пароля)

    bool validateCredentials(const QString &login, const QString &passwordHash); // Проверка логина и хэша пароля
    void sendToClient(QTcpSocket *socket, const QString &message); // Отправка сообщения клиенту
    QMap<QTcpSocket*, quint16> clientBlockSizes;
};

#endif // SERVER_H
