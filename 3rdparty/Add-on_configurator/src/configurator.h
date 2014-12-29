#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QMutex>
#include <QDebug>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFile>
#include <QObject>
#include <QStringList>

class Configurator
{
public:
    static Configurator* instance()
    {
        static QMutex mutex;
        if (!m_Instance)
        {
            mutex.lock();

            if (!m_Instance)
            {
                m_Instance = new Configurator;
            }

            mutex.unlock();
        }

        return m_Instance;
    }

    static void drop()
    {
        static QMutex mutex;
        mutex.lock();
        delete m_Instance;
        m_Instance = 0;
        mutex.unlock();
    }

private:
    Configurator() {}
    Configurator(const Configurator &);
    Configurator& operator=(const Configurator &);
    static Configurator* m_Instance;
    static bool checkDatabaseFile(const QString &basePath);

public:
    bool setDB(const QString &path);
    QString getConfig(const QString &key);
    QMap<QString,QString> getConfigs(QSet<QString> keys);
};


#endif // CONFIGURATOR_H
