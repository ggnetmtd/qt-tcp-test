#ifndef SERVER_H
#define SERVER_H

#include <algorithm>
#include <iostream>
#include <QTcpServer>
#include <QTcpSocket>

#include "database/database.h"
#include "math/mathexpression.h"

class Server : public QObject
{
Q_OBJECT

public:
    QTcpServer m_tcp;
    quint16 m_port;
    QHostAddress m_address;
    QMap<int, QTcpSocket*> m_sockets;

    QFile *m_logfile;
    Database *m_database;

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
};

#endif // SERVER_H
