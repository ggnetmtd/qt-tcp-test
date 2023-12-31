#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql>
#include <chrono>

class Database : public QObject
{
Q_OBJECT

public:
    QSqlDatabase m_db;

    Database(const QString& driver, QObject *parent);


    void init(const QString& dbname)
    {
        setdb(dbname);
    }

    void setdb(const QString& db)
    {
        m_db.setDatabaseName(db);
    }

    bool open()
    {
        return m_db.open();
    }

    void close()
    {
        m_db.close();
    }

    void saveData(const QByteArray& data, const QString& result);

    void createMessagesTable();

signals:
    void gotSqlError(const QString& error);
};

#endif // DATABASE_H
