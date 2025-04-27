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
    connect(clientSocket, &QTcpSocket::disconnected, this, [=]() {
        clients.removeAll(clientSocket);
        authorizedClients.remove(clientSocket);
        clientBlockSizes.remove(clientSocket);
        clientSocket->deleteLater();
        qDebug() << "Клиент отключён:" << socketDescriptor;
    });

    clients.append(clientSocket);
    clientBlockSizes[clientSocket] = 0;
    qDebug() << "Клиент подключен:" << socketDescriptor;
}

void server::slotReadyRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (!clientSocket) return;

    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_6_4);

    while (true) {
        if (clientBlockSizes[clientSocket] == 0) {
            if (clientSocket->bytesAvailable() < sizeof(quint16)) break;
            in >> clientBlockSizes[clientSocket];
        }

        if (clientSocket->bytesAvailable() < clientBlockSizes[clientSocket]) break;

        QString receivedData;
        in >> receivedData;
        clientBlockSizes[clientSocket] = 0;

        qDebug() << "Получены данные от клиента:" << receivedData;

        if (!authorizedClients.contains(clientSocket)) {
            // Обработка авторизации
            if (receivedData.contains(":")) {
                QStringList parts = receivedData.split(":");
                if (parts.size() == 2) {
                    if (validateCredentials(parts[0], parts[1])) {
                        authorizedClients[clientSocket] = parts[0];
                        sendToClient(clientSocket, "SUCCESS");
                        qDebug() << "Клиент авторизован:" << parts[0];
                    } else {
                        sendToClient(clientSocket, "ERROR: Неверный логин или пароль");
                    }
                } else {
                    sendToClient(clientSocket, "ERROR: Некорректный формат данных");
                }
            }
        } else {
            // Обработка сообщений чата
            QString senderName = authorizedClients[clientSocket];
            QString message = senderName + ": " + receivedData;

            for (QTcpSocket *client : clients) {
                if (client != clientSocket && client->state() == QAbstractSocket::ConnectedState) {
                    sendToClient(client, message);
                }
            }
        }
    }
}

bool server::validateCredentials(const QString &login, const QString &passwordHash)
{
    QMap<QString, QString> usersDatabase;
    usersDatabase["1"] = "6b86b273ff34fce19d6b804eff5a3f5747ada4eaa22f1d49c01e52ddb7875b4b"; // Пароль: "1"
    usersDatabase["2"] = "d4735e3a265e16eee03f59718b9b5d03019c07d8b6c51f90da3a666eec13ab35"; // Пароль: "2"

    return usersDatabase.value(login) == passwordHash;
}

void server::sendToClient(QTcpSocket *socket, const QString &message)
{
    if (!socket || socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Попытка отправки на отключенный сокет";
        return;
    }

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);

    out << quint16(0);
    out << message;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));

    qDebug() << "Отправка сообщения:" << message << "Размер:" << block.size();

    if (socket->write(block) == -1) {
        qDebug() << "Ошибка записи в сокет:" << socket->errorString();
    }
    if (!socket->waitForBytesWritten(1000)) {
        qDebug() << "Таймаут отправки данных";
    }
}
