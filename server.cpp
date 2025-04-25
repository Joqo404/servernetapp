#include "server.h"
#include <QCryptographicHash>
#include <QDebug>

server::server()
{
    if (this->listen(QHostAddress::LocalHost, 5555)) {
        qDebug() << "Сервер запущен";
    } else {
        qDebug() << "Ошибка запуска сервера";
    }
}

void server::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *clientSocket = new QTcpSocket;
    clientSocket->setSocketDescriptor(socketDescriptor);
    connect(clientSocket, &QTcpSocket::readyRead, this, &server::slotReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
    qDebug() << "Клиент подключен:" << socketDescriptor;
}

void server::slotReadyRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_6_4);

    QString receivedData;
    in >> receivedData;

    qDebug() << "Получены данные от клиента:" << receivedData;

    // Разделяем полученные данные на логин и хэш пароля
    QStringList parts = receivedData.split(":");
    if (parts.size() != 2) {
        qDebug() << "Некорректный формат данных";
        sendToClient(clientSocket, "ERROR: Некорректный формат данных");
        return;
    }

    QString login = parts[0];
    QString passwordHash = parts[1];

    // Проверяем логин и хэш пароля
    if (validateCredentials(login, passwordHash)) {
        qDebug() << "Авторизация успешна для пользователя:" << login;
        sendToClient(clientSocket, "SUCCESS");
    } else {
        qDebug() << "Неверный логин или пароль для пользователя:" << login;
        sendToClient(clientSocket, "ERROR: Неверный логин или пароль");
    }
}

bool server::validateCredentials(const QString &login, const QString &passwordHash)
{
    // Здесь можно добавить базу данных или жестко заданные логины/пароли
    QMap<QString, QString> usersDatabase; // Логин -> Хэш пароля
    usersDatabase["user1"] = QByteArray::fromHex("5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8").toHex(); // Пароль: "password"
    usersDatabase["1"] = QByteArray::fromHex("6b86b273ff34fce19d6b804eff5a3f5747ada4eaa22f1d49c01e52ddb7875b4b").toHex(); // Пароль: "1"

    if (usersDatabase.contains(login) && usersDatabase[login] == passwordHash) {
        return true;
    }
    return false;
}

void server::sendToClient(QTcpSocket *socket, const QString &message)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    out << message;
    socket->write(data);
}
