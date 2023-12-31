#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <QTcpServer>
#include <QTcpSocket>

#include "database/database.h"
#include "math/mathexpression.h"

#define MATH_HEADER_BYTE 1


class Server : public QObject
{
Q_OBJECT

public:
    const QString database_file = "test.sqlite";
    const QString log_file = "log.txt";

    QTcpServer m_tcp;
    quint16 m_port;
    QHostAddress m_address;
    QMap<int, QTcpSocket*> m_sockets;

    QFile m_logfile;
    Database m_database;

    bool isSqlEnabled = true;

    Server(std::string addr, uint16_t port, QObject *parent);
    ~Server();
    void init();
    void run();
public slots:
    void logSqlError(const QString& error);
    void logDisconnect();
    void handleConnection();
    void handle();
private:
    void log(const QString& msg);
    void sendData(const QString& msg, QTcpSocket* socket);
};

#endif // SERVER_H
