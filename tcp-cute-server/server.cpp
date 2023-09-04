#include "server.h"

Server::Server(std::string addr, uint16_t port, QObject *parent = Q_NULLPTR) : QObject(parent),
    m_logfile(this->log_file), m_database("QSQLITE", this)
{
    m_address = QHostAddress(QString::fromStdString(addr));
    m_port = port;

    if(!m_logfile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        this->log("Can't open logfile.");
    }

    // SQL init
    if(isSqlEnabled)
    {
        m_database.init(database_file);

        connect(&m_database, &Database::gotSqlError, this, &logSqlError);
    }
}

Server::~Server()
{
    foreach(int i, m_sockets.keys())
    {
        m_sockets[i]->close();
        m_sockets.remove(i);
    }

    m_tcp.close();

    log("Server was shutdown.");

    m_database.close();
    m_logfile.close();
}

void Server::log(const QString& msg)
{
    QDateTime date(QDateTime::currentDateTime());

    QString log_msg = date.toString() + " [Server]: " + msg + "\n";

    QTextStream lout(&m_logfile);

    lout << log_msg;

    std::cout << "[Server]: " << msg.toStdString() << std::endl;
}

void Server::run()
{   
    log("Running at " + m_address.toString() + ":" + QString::number(m_port));
    m_tcp.listen(m_address, m_port);

    if(isSqlEnabled)
    {
        if(!m_database.open())
        {
            // m_database->m_db.lastError().text();
            log("Can't connect to SQLite database.");
        }
        else
        {
            // открыли базу, если нет таблицы, создаем
            m_database.createMessagesTable();
        }
    }
}

void Server::logSqlError(const QString &error)
{
    log(error);
}

void Server::init()
{
    connect(&m_tcp, &QTcpServer::newConnection, this, &handleConnection);

    log("Initialization is completed.");
}

void Server::handleConnection()
{
    QTcpSocket* sock = m_tcp.nextPendingConnection();
    int sockDesc = sock->socketDescriptor();

    log("New connection. <" + sock->peerAddress().toString() + ":" + QString::number(sock->peerPort()) + ">");

    m_sockets[sockDesc] = sock;
    connect(m_sockets[sockDesc], &QTcpSocket::readyRead, this, &Server::handle);
    connect(m_sockets[sockDesc], &QTcpSocket::disconnected, this, &Server::logDisconnect);
}

void Server::logDisconnect()
{
    QTcpSocket* sock = (QTcpSocket*) sender();

    log("Disconnected. <" + sock->peerAddress().toString() + ":" + QString::number(sock->peerPort()) + ">");

    // close & remove socket from the map (if it exists)
    //    m_sockets[sockDesc]->close();
    //    m_sockets.remove(sockDesc);
}

void Server::handle()
{
    QTcpSocket* sock = (QTcpSocket*) sender();

    QByteArray data = sock->readAll();

    QByteArray toSend;

    // ...
    char header = data[0];

    QString message = QString(data);
    log("Message: " + message);

    // Todo: refactor to another method
    if(header == MATH_HEADER_BYTE)
    {
        log("Header is alright.");

        // Removed header and whitespaces.
        data.removeFirst();
        data.erase(std::remove_if(data.begin(), data.end(), isspace), data.end());

        log("Removed whitespaces.");

        // Somewhat empty string
        if(data.size() < 1)
        {
            const QString MSG_IS_TOO_SHORT = "Request message is probably empty.";

            log(MSG_IS_TOO_SHORT);

            toSend.append(MATH_HEADER_BYTE); // header
            toSend.push_back(MSG_IS_TOO_SHORT.toUtf8()); // msg

            m_database.saveData(data, MSG_IS_TOO_SHORT);

            sock->write(toSend);

            return;
        }

        std::string dataString = QString(data).toStdString();

        std::unique_ptr<MathExpression> expr;
        double result;
        QString resultStr;

        try
        {
            expr = std::make_unique<MathExpression>(dataString);
            result = expr->calculate();
            resultStr = QString::number(result);
        }
        catch (std::invalid_argument& err)
        {
            log(err.what());
            resultStr = QString(err.what());
        }

        toSend.append(MATH_HEADER_BYTE); // header
        toSend.push_back(resultStr.toUtf8()); // msg

        log("Result:" + resultStr);

        if(isSqlEnabled)
            m_database.saveData(data, resultStr);

        sock->write(toSend);
    }
    else
    {
        const QString MSG_INVALID_HEADER = "Request header is invalid.";
        toSend.append(MATH_HEADER_BYTE);
        toSend.push_back(MSG_INVALID_HEADER.toUtf8());

        sock->write(toSend);
    }
}
