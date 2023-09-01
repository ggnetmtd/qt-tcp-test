#ifndef CONNECTION_H
#define CONNECTION_H

#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>

#include <QMainWindow>
#include <QMessageBox>
#include <QStatusBar>

#include <chrono>
#include <cinttypes>
#include <string>

#include "msgheader.h"

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 8080


class Connection : public QObject
{
Q_OBJECT

private:
    QTcpSocket m_socket;
    QHostAddress m_ipAddress;
    uint16_t m_port;
    void initSocket();

private slots:
    void receive();
    void displayError(QAbstractSocket::SocketError socketError);
    void displayError(char mtype);

signals:
    void invalidHeader(char mtype);
    void gotResult(QByteArray res);
    void changedStatus(QString text);

public:
    Connection(const std::string& ip, const uint16_t port, QObject *parent);
    ~Connection();

    void open();
    void close();

    bool isConnected() const;
    void send(const MessageHeader& header, const std::string& msg);

    QTcpSocket* GetSocket() {
        return &m_socket;
    }
};

#endif // CONNECTION_H
