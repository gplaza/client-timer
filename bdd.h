#ifndef BDD_H
#define BDD_H

#include <QtSql>
#include <QStringList>
#include <QFile>
#include <configurator.h>
#include <acceso.h>
#include <persona.h>

class AccessWorker : public QObject
{
    Q_OBJECT
public:
    AccessWorker(int ErrorType = 0)
    {
        this->ErrorType = ErrorType;
    }

public slots:
    void process()
    {
        QSqlQuery query(QSqlDatabase::database("acceso"));

        QString sql;
        int access = 1;
        int idStat = 0;
        QDate date = QDate::currentDate();
        int num_week = date.weekNumber();
        int num_day = date.dayOfWeek();

        QString sql_select = "SELECT acc,id FROM stat_week WHERE num_week=:num_week AND num_day=:num_day";
        query.prepare(sql_select);
        query.bindValue(":num_week",num_week);
        query.bindValue(":num_day",num_day);

        if (!query.exec())
            qCritical() << "Query Error (AccessWorker.sql_select.process) : " << query.lastError();

        if(query.first())
        {
            QSqlRecord statWeek = query.record();
            access = statWeek.value("acc").toInt() + access;
            idStat = statWeek.value("id").toInt();
            sql = "UPDATE stat_week SET acc=:acc WHERE num_week=:num_week AND num_day=:num_day";

        } else {

            sql = "INSERT INTO stat_week(acc,num_week,num_day) VALUES (:acc, :num_week, :num_day);";
        }

        query.prepare(sql);
        query.bindValue(":acc",access);
        query.bindValue(":num_week",num_week);
        query.bindValue(":num_day",num_day);

        if (!query.exec())
            qCritical() << "Query Error (AccessWorker.sql.process) : " << query.lastError();
        else
            qDebug() << "Register access : " << "total : " << access << " week number : " << num_week << " day number : " << num_day;

        if(ErrorType != 0)
        {
            int number = 1;

            if(idStat == 0)
                idStat = query.lastInsertId().toInt();

            QString sql_select_error = "SELECT number FROM stat_error WHERE error_id=:error_id AND stat_id=:stat_id";
            query.prepare(sql_select_error);
            query.bindValue(":error_id",ErrorType);
            query.bindValue(":stat_id",idStat);

            if (!query.exec())
                qCritical() << "Query Error (AccessWorker.sql_select_error.process) : " << query.lastError();

            if(query.first())
            {
                number =  query.value(0).toInt() + number;
                sql = "UPDATE stat_error SET number=:number WHERE error_id=:error_id AND stat_id=:stat_id";

            } else {

                sql = "INSERT INTO stat_error(number,error_id,stat_id) VALUES (:number, :error_id, :stat_id)";
            }

            query.prepare(sql);
            query.bindValue(":number",number);
            query.bindValue(":error_id",ErrorType);
            query.bindValue(":stat_id",idStat);

            if (!query.exec())
                qCritical() << "Query Error (AccessWorker.sql.process) : " << query.lastError();
            else
                qDebug() << "Register access Error : " << "ErrorType : " << ErrorType;
        }

        emit finished();
    }

signals:
    void finished();

private:
    int ErrorType;

};

class Bdd : public QObject
{
    Q_OBJECT
public:
    Bdd(QObject *parent = 0);

    static QSqlRecord identificationCredencial(QString uuid);
    static QSqlRecord identificationFingerprint(int id);
    static QSqlRecord identificationOffline(QString rut);

    static QString textAuthentication(Acceso &acceso);
    static void saveAccess(Acceso &acceso, Persona &persona);
    static void registerAccess(int ErrorType = 0);
    static void deleteAccess(Acceso *acceso);
    static Acceso *syncAccess();
    static bool checkOfflineData();

    static void updatePersona(Persona &persona);
    static void updatePersonaByAcceso(Acceso &acceso);
    static void createPersona(Acceso &acceso);
    static bool checkPersona(Acceso &acceso);

    static QByteArray getImage(QString rut);
    static void setImage(QString rut, QByteArray img);

    void openDatabase();
    bool checkDatabaseFile(const QString &basePath);
    void createDatabase(const QString &scriptPath, const QString &conn);
    void createTables(const QString &sqlLine, QSqlDatabase db);

    enum ErrorAccess {
        WEBSERVICE_ERROR = 1,
        TIMEOUT_FINGERPRINT = 2,
        UNKNOWN_FINGERPRINT = 3
    };

signals:
    void bddReady();

};

#endif // BDD_H
