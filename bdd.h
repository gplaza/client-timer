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
    AccessWorker(int ErrorType = 0, Acceso *acceso = NULL)
    {
        this->ErrorType = ErrorType;
        this->acceso = acceso;
    }

public slots:
    void process()
    {
        QSqlQuery query(QSqlDatabase::database("acceso"));

        QString sql;
        int access = 1;
        int idStat = 0;

        QDate date = QDate::currentDate();
        int year = date.year();
        int current_week = date.weekNumber();
        int current_day = date.dayOfWeek();
        int current_year = date.year();
        bool newYear = false;

        QString sql_select = "SELECT acc, id, num_year FROM stat_week WHERE num_week=:current_week AND num_day=:current_day";
        query.prepare(sql_select);
        query.bindValue(":current_week",current_week);
        query.bindValue(":current_day",current_day);

        if (!query.exec())
            qCritical() << "Query Error (AccessWorker.sql_select.process) : " << query.lastError();

        if(query.first())
        {
            QSqlRecord statWeek = query.record();

            year = statWeek.value("num_year").toInt();
            newYear = current_year != year;
            access = (newYear? 0 : statWeek.value("acc").toInt()) + access;
            idStat = statWeek.value("id").toInt();

            sql = "UPDATE stat_week SET acc=:access, num_year=:current_year WHERE num_week=:current_week AND num_day=:current_day";

        } else {

            sql = "INSERT INTO stat_week(acc, num_week, num_day, num_year) VALUES (:access, :current_week, :current_day, :current_year);";
        }

        query.prepare(sql);
        query.bindValue(":access",access);
        query.bindValue(":current_week",current_week);
        query.bindValue(":current_day",current_day);
        query.bindValue(":current_year",current_year);

        if (!query.exec())
            qCritical() << "Query Error (AccessWorker.sql.process) : " << query.lastError();
        else
            qDebug() << "Register access : " << "total : " << access << " week number : " << current_week << " day number : " << current_day << " year : " << current_year;

        if(idStat == 0)
            idStat = query.lastInsertId().toInt();

        if(newYear)
        {
            QString sql_delete_error = "DELETE FROM stat_error WHERE stat_id=:stat_id";
            query.prepare(sql_delete_error);
            query.bindValue(":stat_id",idStat);
            query.exec();
        }

        if(ErrorType != 0)
        {
            int number = 1;

            QString sql_select_error = "SELECT number FROM stat_error WHERE error_id=:error_id AND stat_id=:stat_id";
            query.prepare(sql_select_error);
            query.bindValue(":error_id",ErrorType);
            query.bindValue(":stat_id",idStat);

            if (!query.exec())
                qCritical() << "Query Error (AccessWorker.sql_select_error.process) : " << query.lastError();

            if(query.first())
            {
                number = query.value(0).toInt()  + number;
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

        if(this->acceso != NULL)
        {

            // TODO : change position after refactoring :

            QDateTime dateTime = QDateTime::currentDateTime();

            sql = "INSERT INTO access(rut,date) VALUES (:rut, :date)";

            query.prepare(sql);
            query.bindValue(":rut",this->acceso->rut());
            query.bindValue(":date",dateTime.toString("yyyy-MM-dd hh:mm:ss"));

            if (!query.exec())
                qCritical() << "Query Error (AccessWorker.sql.process.acceso) : " << query.lastError();
        }

        emit finished();
    }

signals:
    void finished();

private:
    int ErrorType;
    Acceso *acceso;

};

class Bdd : public QObject
{
    Q_OBJECT
public:
    Bdd(QObject *parent = 0);

    static QSqlRecord identificationCredencial(QString uuid);
    static QSqlRecord identificationFingerprint(QString id);
    static QSqlRecord identificationOfflineByRut(QString rut);
    static QSqlRecord identificationOfflineByUuid(QString uuid);

    static QString textAuthentication(Acceso *acceso);
    static int casinoService();

    static QSqlQuery getHorarios(int dia, int periodo = 1);

    // BDD control Syncro
    static void saveAccess(Acceso *acceso, Persona &persona);
    static void registerAccess(int ErrorType = 0, Acceso *acceso = NULL);
    static void deleteAccess(Persona &persona, QDateTime date);
    static QDateTime syncAccess(Persona &persona);
    static bool checkOfflineData();

    static void updatePersona(Persona &persona);
    static void updatePersonaByAcceso(Acceso *acceso);
    static void updateCasinoService(Acceso *acceso);
    static void createPersona(Acceso *acceso);
    static bool checkPersona(Acceso *acceso);

    void openDatabase();
    bool checkDatabaseFile(const QString &basePath);
    void createDatabase(const QString &scriptPath, const QString &conn);
    void createTables(const QString &sqlLine, QSqlDatabase db);

    enum ErrorAccess {
        WEBSERVICE_ERROR = 1,
        TIMEOUT_FINGERPRINT = 2,
        UNKNOWN_FINGERPRINT = 3
    };

private:
    bool saveDatabase(QString databaseName, QString fileName);

signals:
    void bddReady();
private slots:
    void saveDB();
public slots:
    void dataBaseChanged();


};

#endif // BDD_H
