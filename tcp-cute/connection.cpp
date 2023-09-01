#include "connection.h"

#define ASOCKET_ERROR QAbstractSocket::SocketError

// #define SET_STATUS(text) static_cast<QMainWindow*>(parent())->statusBar()->showMessage(text);


Connection::Connection(const std::string& ip = DEFAULT_HOST, const uint16_t port = DEFAULT_PORT, QObject *parent = Q_NULLPTR) : QObject(parent)
{
    m_ipAddress = QHostAddress(QString::fromStdString(ip));
    m_port = port;

    try
    {
        this->initSocket();
    }
    catch(std::invalid_argument& err)
    {
        // show me something
        // err.what();
    }
}


void Connection::initSocket()
{
    connect(&m_socket, &QTcpSocket::readyRead, this, &Connection::receive);
    connect(&m_socket, &QAbstractSocket::errorOccurred, this, qOverload<ASOCKET_ERROR>(&Connection::displayError));
    connect(this, &Connection::invalidHeader, this, qOverload<char>(&Connection::displayError) );
}

void Connection::open()
{
    if(m_ipAddress.isNull())
        throw std::invalid_argument("IP address is invalid.");

    m_socket.connectToHost(m_ipAddress, m_port, QIODevice::ReadWrite);

    emit changedStatus("Connecting...");
}

void Connection::close()
{
    if(m_socket.isOpen())
        m_socket.close();
}

Connection::~Connection()
{
    close();
}


void Connection::displayError(QAbstractSocket::SocketError socketError)
{
    const QString msgText = "SocketError: " + QVariant::fromValue(socketError).toString();
    QMessageBox::critical(qobject_cast<QWidget*>(this),
                          tr("Network problem accured"),
                          msgText);

    emit changedStatus("Disconnected.");
}

void Connection::displayError(char mtype)
{
    QMessageBox::critical(qobject_cast<QWidget*>(this),
                          tr("Received message type is invalid"),
                          tr("Type: " + mtype));

    emit changedStatus("Disconnected.");
}

void Connection::receive()
{
    QByteArray data = m_socket.readAll();

    emit changedStatus("Got some data... checking the header.");

    char header = data[0];
    MessageType htype = Unknown;

    if(!MessageHeader::castByte(header, &htype))
    {
        emit invalidHeader(header);

        return;
    }

    emit gotResult(data);
}

void Connection::send(const MessageHeader& header, const std::string& msg)
{

    emit changedStatus("Sending data...");

    QByteArray data = QByteArray::fromStdString(msg);

    data.push_front( header.getByte() );

    m_socket.write(data);
}

bool Connection::isConnected() const
{
    return m_socket.state() == m_socket.ConnectedState;
}
