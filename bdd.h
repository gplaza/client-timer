#ifndef BDD_H
#define BDD_H

#include <QtSql>
#include <QStringList>
#include <QFile>
#include <configurator.h>
#include <acceso.h>
#include <persona.h>

class AccessWorker3 : public QObject
{
    Q_OBJECT
public:
    AccessWorker3(QString rut, int estado, int tipoAcceso, int tipoCredencial)
    {
        this->rut = rut;
        this->estado = estado;
        this->tipoAcceso = tipoAcceso;
        this->tipoCredencial = tipoCredencial;
    }

public slots:
    void process()
    {
        // TODO : change position after refactoring :

        QDateTime dateTime = QDateTime::currentDateTime();

        QSqlQuery query(QSqlDatabase::database("acceso"));
        QString sql = "INSERT INTO access(rut, estado, tipoAcceso, tipoCredencial, date) VALUES (:rut, :estado, :tipoAcceso, :tipoCredencial, :date)";

        query.prepare(sql);
        query.bindValue(":rut",rut);
        query.bindValue(":estado",estado);
        query.bindValue(":tipoAcceso",tipoAcceso);
        query.bindValue(":tipoCredencial",tipoCredencial);
        query.bindValue(":date",dateTime.toString("yyyy-MM-dd hh:mm:ss"));

        if (!query.exec())
            qCritical() << "Query Error (AccessWorker.sql.process.acceso) : " << query.lastError();

        emit finished();
    }

signals:
    void finished();

private:
    QString rut;
    int estado;
    int tipoAcceso;
    int tipoCredencial;

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

    // static void registerAccess2(QString rut);
    // static void registerAccess(int ErrorType = 0);
    static void registerAccess3(QString rut, int estado, int tipoAcceso, int tipoCredencial);

    static void deleteAccess(Persona &persona, QDateTime date);
    static QDateTime syncAccess(Persona &persona);
    static bool checkOfflineData();

    static void updatePersona(Persona &persona);
    static void updatePersonaByAcceso(Acceso *acceso);
    static void updateCasinoService(Acceso *acceso);
    static void createPersona(Acceso *acceso);
    static bool checkPersona(Acceso *acceso);
    static QMap<QString,QString> getUserOption(int UserID);

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
