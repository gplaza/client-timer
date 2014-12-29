#include "configurator.h"

Configurator* Configurator::m_Instance = 0;

bool Configurator::setDB(const QString &path)
{
    bool newConfigDB = checkDatabaseFile(path);

    if(!newConfigDB)
    {
        if(!QSqlDatabase::database("config").isValid())
        {
            QSqlDatabase accesoBDD = QSqlDatabase::addDatabase("QSQLITE","config");
            accesoBDD.setDatabaseName(path);
            accesoBDD.open();

            return true;
        }
    }

    return false;
}

QMap<QString,QString> Configurator::getConfigs(QSet<QString> keys)
{
    QSqlQuery query(QSqlDatabase::database("config"));
    QMap <QString,QString> result;
    QStringList keysString;

    QString sql = "SELECT key, value FROM config WHERE ";

    foreach (QString key, keys)
        keysString << " key = '" + key + "'";

    sql += keysString.join(" OR ");

    if (!query.exec(sql))
        qCritical() << "Query Error (getConfigs) : " << query.lastError();

    while(query.next()) {
        QString key = query.value(0).toString();
        QString value = query.value(1).toString();
        result.insert(key,value);
    }

    return result;
}

QString Configurator::getConfig(const QString &key)
{
    QSqlQuery query(QSqlDatabase::database("config"));

    QString sql = "SELECT value FROM config WHERE key=:key LIMIT 1";
    query.prepare(sql);
    query.bindValue(":key", key);

    if (!query.exec())
        qCritical() << "Query Error (getConfig) : " << query.lastError();

    if (query.first())
        return query.value(0).toString();

    return "";
}

bool Configurator::checkDatabaseFile(const QString &basePath)
{
    QFile databaseFile(basePath);

    if (!databaseFile.exists())
    {
        qDebug() << "Database Configurator " << databaseFile.fileName() << " not exist";
        return true;
    }

    qDebug() << "Database Configurator " << databaseFile.fileName() << " exist";
    return false;
}
