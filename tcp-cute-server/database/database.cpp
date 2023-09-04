#include "database.h"

Database::Database(const QString& driver, QObject *parent = Q_NULLPTR) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase(driver);
}

void Database::saveData(const QByteArray &data, const QString &result)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO messages (date, received, result)"
                  "VALUES (:date, :received, :result)");

    // current time
    const auto now = std::chrono::system_clock::now();
    const auto tt = std::chrono::system_clock::to_time_t(now);

    query.bindValue(":date", tt);
    query.bindValue(":received", data);
    query.bindValue(":result", result);

    try
    {
        bool result = query.exec();

        if(!result)
            emit gotSqlError(query.lastError().text());
    }
    catch (QSqlError &err)
    {
        emit gotSqlError(err.text());
    }
}


void Database::createMessagesTable()
{
    const QString table_name = "messages";

    QSqlQuery query(m_db);

    query.prepare(QString("CREATE TABLE IF NOT EXISTS %1 "
              "(id integer not null primary key, "
              "date datetime,"
              "received varchar(255),"
              "result varchar(255))").arg(table_name));

    try
    {
        bool result = query.exec();

        if(!result)
            emit gotSqlError(query.lastError().text());
    }
    catch (QSqlError &err)
    {
        emit gotSqlError(err.text());
    }
}
