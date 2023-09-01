#include "server.h"

Server::Server(std::string addr, uint16_t port, QObject *parent = Q_NULLPTR) : QObject(parent)
{
    m_address = QHostAddress(QString::fromStdString(addr));
    m_port = port;

    // SQL init
    if(isSqlEnabled)
    {
        m_database = new Database("QSQLITE", this); // delete in the destructor

        m_database->setdb("test.sqlite");

        // m_database->init("127.0.0.1", "admin", "admin");
        // m_database->init();

        connect(m_database, &Database::gotSqlError, this, &logSqlError);
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

    delete m_database;
}

void Server::log(const QString& msg)
{
    std::cout << "[Server]: " << msg.toStdString() << std::endl;
}

void Server::run()
{
    log("Running at " + m_address.toString() + ":" + QString::number(m_port));
    m_tcp.listen(m_address, m_port);

    if(isSqlEnabled)
    {
        if(!m_database->open())
        {
            // m_database->m_db.lastError().text();
            log("Can't connect to SQLite database.");
        }
        else
        {
            // открыли базу, если нет таблицы, создаем
            m_database->createMessagesTable();
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

    log("New connection. <" + sock->peerAddress().toString() + ">");

    m_sockets[sockDesc] = sock;
    connect(m_sockets[sockDesc], &QTcpSocket::readyRead, this, &Server::handle);
    connect(m_sockets[sockDesc], &QTcpSocket::disconnected, this, &Server::logDisconnect);
}

void Server::logDisconnect()
{
    QTcpSocket* sock = (QTcpSocket*) sender();
    int sockDesc = sock->socketDescriptor();

    log("Disconnected. <" + sock->peerAddress().toString() + ">");

    // close & remove socket from the map (if it exists)
    try
    {
        m_sockets[sockDesc]->close();
        m_sockets.remove(sockDesc);
    }
}

void Server::handle()
{
    QTcpSocket* sock = (QTcpSocket*) sender();
    int sockDesc = sock->socketDescriptor();

    QByteArray data = sock->readAll();

    // ...
    char header = data[0];

    QString message = QString(data);
    log("Message: " + message);

    // Todo: refactor to another method
    if(header == 1)
    {
        log("Header is alright.");
        QByteArray toSend;

        // Removed header and whitespaces.
        data.removeFirst();
        data.erase(std::remove_if(data.begin(), data.end(), isspace), data.end());

        log("Removed whitespaces.");

        // Somewhat empty string
        if(data.size() < 1)
        {
            const QString MSG_IS_TOO_SHORT = "Request message is probably empty.";

            log(MSG_IS_TOO_SHORT);

            toSend.append(1); // header
            toSend.push_back(MSG_IS_TOO_SHORT.toUtf8()); // msg

            m_database->saveData(data, MSG_IS_TOO_SHORT);

            sock->write(toSend);

            return;
        }

        std::string dataString = QString(data).toStdString();

        MathExpression expr(dataString);

        double result;
        QString resultStr;

        try {
            result = expr.calculate();
            resultStr = QString::number(result);
        }
        catch(std::invalid_argument &err)
        {
            resultStr = QString(err.what());
        }

        toSend.append(1); // header
        toSend.push_back(resultStr.toUtf8()); // msg

        log("Result:" + resultStr);

        if(isSqlEnabled)
            m_database->saveData(data, resultStr);

        sock->write(toSend);
    }
    else
    {

    }
}
