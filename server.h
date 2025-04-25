#ifndef SERVER_H
#define SERVER_H

#include <QTcpSocket>
#include <QTcpServer>
#include <QMap>
#include <QCryptographicHash>
#include <QDebug>

class server : public QTcpServer
{
    Q_OBJECT

public:
    server();

private:
    QByteArray data;
    QMap<QString, QString> usersDatabase; // База данных пользователей (логин -> хэш пароля)

    // Метод для проверки логина и хэша пароля
    bool validateCredentials(const QString &login, const QString &passwordHash);

    // Метод для отправки данных клиенту
    void sendToClient(QTcpSocket *socket, const QString &message);

public slots:
    void slotReadyRead(); // Слот для обработки входящих данных
    void incomingConnection(qintptr socketDescriptor) override; // Обработка нового подключения
};

#endif // SERVER_H
