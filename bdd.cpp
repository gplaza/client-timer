#include "bdd.h"
#include <sqlite3.h>

static QString dbFile = "";

Bdd::Bdd(QObject *parent) : QObject(parent)
{

}

void Bdd::openDatabase()
{
    bool newAccesoDB = checkDatabaseFile("/usr/share/nginx/www/protected/data/acceso.db");
    bool newConfigDB = checkDatabaseFile("/usr/share/nginx/www/protected/data/config.db");
    bool newSyncroDB = checkDatabaseFile("/usr/share/nginx/www/protected/data/offline.db");

    QSqlDatabase dbSyncro = QSqlDatabase::addDatabase("QSQLITE","syncro");
    dbSyncro.setDatabaseName("/usr/share/nginx/www/protected/data/offline.db");

    if(!dbSyncro.open())
        qCritical() << "Open Syncro BDD error : " << dbSyncro.lastError();

    QSqlDatabase dbAcceso = QSqlDatabase::addDatabase("QSQLITE","acceso");
    dbAcceso.setDatabaseName("/usr/share/nginx/www/protected/data/acceso.db");

    if(!dbAcceso.open())
        qCritical() << "Open Acceso BDD error : " << dbAcceso.lastError();

    if(!QSqlDatabase::database("config").isValid())
    {
        QSqlDatabase dbConfig = QSqlDatabase::addDatabase("QSQLITE","config");
        dbConfig.setDatabaseName("/usr/share/nginx/www/protected/data/config.db");

        if(!dbConfig.open())
            qCritical() << "Open Config BDD error : " << dbConfig.lastError();
    }

    if(newAccesoDB)
        createDatabase("/usr/share/nginx/www/protected/data/schema.acceso.sql","persona");
    if(newConfigDB)
        createDatabase("/usr/share/nginx/www/protected/data/schema.config.sql","config");
    if(newSyncroDB)
        createDatabase("/usr/share/nginx/www/protected/data/schema.offline.sql","syncro");
}

QSqlRecord Bdd::identificationCredencial(QString uuid)
{
    QSqlQuery query(QSqlDatabase::database("acceso"));
    QSqlRecord result;

    QString sql = "SELECT rut,id_huella FROM persona WHERE uuid=:uuid";

    query.prepare(sql);
    query.bindValue(":uuid", uuid);

    if (!query.exec())
        qCritical() << "Query Error (identificationCredencial) : " << query.lastError();

    if(query.first())
        result = query.record();

    return result;
}

QSqlRecord Bdd::identificationFingerprint(QString id)
{
    QSqlQuery query(QSqlDatabase::database("acceso"));
    QSqlRecord result;

    QString sql = "SELECT hash,rut FROM persona WHERE id_huella=:id_huella";

    query.prepare(sql);
    query.bindValue(":id_huella", id);

    if (!query.exec())
        qCritical() << "Query Error (identificationFingerprint) : " << query.lastError();

    if(query.first())
        result = query.record();

    return result;
}

QSqlRecord Bdd::identificationOfflineByRut(QString rut)
{
    QSqlQuery query(QSqlDatabase::database("acceso"));
    QSqlRecord result;

    QString sql = "SELECT id_huella,autorizado,nombre FROM persona WHERE rut=:rut";

    query.prepare(sql);
    query.bindValue(":rut", rut);

    if (!query.exec())
        qCritical() << "Query Error (identificationOfflineByRut) : " << query.lastError();

    if(query.first())
        result = query.record();

    return result;
}

QSqlRecord Bdd::identificationOfflineByUuid(QString uuid)
{
    QSqlQuery query(QSqlDatabase::database("acceso"));
    QSqlRecord result;

    QString sql = "SELECT autorizado,nombre FROM persona WHERE uuid=:uuid";

    query.prepare(sql);
    query.bindValue(":uuid", uuid);

    if (!query.exec())
        qCritical() << "Query Error (identificationOfflineByUuid) : " << query.lastError();

    if(query.first())
        result = query.record();

    return result;
}

QString Bdd::textAuthentication(Acceso *acceso)
{
    QSqlQuery query(QSqlDatabase::database("config"));
    QString result = "";

    QString sql = "SELECT descripcion FROM estado WHERE valor=:valor";

    query.prepare(sql);
    query.bindValue(":valor", acceso->idAuth());

    if (!query.exec())
        qCritical() << "Query Error (getTextAuthentication) : " << query.lastError();

    if(query.first())
        result =  query.value(0).toString();

    return result;
}

bool Bdd::checkOfflineData()
{
    QSqlQuery query(QSqlDatabase::database("syncro"));

    if (!query.exec("SELECT fecha FROM acceso LIMIT 1"))
        qCritical() << "Query Error (checkOfflineData) : " << query.lastError();

    return query.first();
}

void Bdd::saveAccess(Acceso *acceso, Persona &persona)
{
    QSqlQuery query(QSqlDatabase::database("syncro"));

    QString sql = "INSERT INTO acceso(uuid, rut, fecha) VALUES (:uuid, :rut, :fecha);";

    query.prepare(sql);

    query.bindValue(":uuid", persona.uuid());
    query.bindValue(":fecha", acceso->dateFormated("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":rut", persona.rut());

    if (!query.exec())
        qCritical() << "Query Error (SaveAccess) : " << query.lastError();
}

QDateTime Bdd::syncAccess(Persona &persona)
{
    QSqlQuery query(QSqlDatabase::database("syncro"));
    QDateTime date;

    if (!query.exec("SELECT fecha, tipoMarca, uuid, rut FROM acceso ORDER BY fecha LIMIT 1"))
        qCritical() << "Query Error (syncAccess) : " << query.lastError();

    if(query.first())
    {
        QSqlRecord access = query.record();

        date = QDateTime::fromString(access.value("fecha").toString(),"yyyy-MM-dd hh:mm:ss");
        int tipoMarca = access.value("tipoMarca").toInt();
        QString uuid = access.value("uuid").toString();
        QString rut = access.value("rut").toString();

        persona.setRut(rut);
        persona.setUuid(uuid);
        persona.setTipoMarca(tipoMarca);
    }

    return date;
}

QSqlQuery Bdd::getHorarios(int dia, int periodo)
{
    QSqlDatabase db = QSqlDatabase::database("acceso");
    QSqlQuery query(db);

    QString sql = "SELECT dia,horario_inicio,horario_fin FROM horario WHERE periodo_id=:periodo AND dia=:dia";

    query.prepare(sql);
    query.bindValue(":periodo", periodo);
    query.bindValue(":dia", dia);

    if (!query.exec())
        qCritical() << "Query Error (getPeriodo) : " << query.lastError();

    return query;
}

void Bdd::deleteAccess(Persona &persona, QDateTime date)
{
    QSqlQuery query(QSqlDatabase::database("syncro"));

    QString sql = "DELETE FROM acceso WHERE fecha=:fecha AND tipoMarca=:tipoMarca;";
    query.prepare(sql);

    query.bindValue(":fecha", date.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":tipoMarca", persona.tipoMarca());

    if (!query.exec())
        qCritical() << "Query Error (deleteAccess) : " << query.lastError();
}

bool Bdd::checkPersona(Acceso *acceso)
{
    QSqlQuery query(QSqlDatabase::database("acceso"));

    QString sql = "SELECT uuid,id_huella FROM persona WHERE rut=:rut";
    query.prepare(sql);
    query.bindValue(":rut", acceso->rut());

    if (!query.exec())
        qCritical() << "Query Error (checkPersona) : " << query.lastError();

    return query.first();
}

void Bdd::createPersona(Acceso *acceso)
{
    QSqlQuery query(QSqlDatabase::database("acceso"));

    QString sql = "INSERT INTO persona(autorizado,nombre,rut,uuid) VALUES (:autorizado,:nombre,:rut,:uuid);";
    query.prepare(sql);

    query.bindValue(":autorizado", acceso->idAuth());
    query.bindValue(":nombre", acceso->name());
    query.bindValue(":uuid", acceso->uuid());
    query.bindValue(":rut", acceso->rut());

    if (!query.exec())
        qCritical() << "Query Error (createPersona) : " << query.lastError();
}

void Bdd::updatePersona(Persona &persona)
{
    QSqlQuery query(QSqlDatabase::database("acceso"));

    QString sql = "UPDATE persona SET id_huella=:idh,hash=:hash WHERE rut=:rut;";
    query.prepare(sql);

    query.bindValue(":idh", persona.fingerprintID());
    query.bindValue(":hash", persona.hash());
    query.bindValue(":rut", persona.rut());

    if (!query.exec())
        qCritical() << "Query Error (createPersona) : " << query.lastError();
}

void Bdd::registerAccess2(QString rut)
{
    QThread *thread = new QThread;
    AccessWorker2* accessWorker2 = new AccessWorker2(rut);
    accessWorker2->moveToThread(thread);

    //Start Thread :
    connect(thread, &QThread::started, accessWorker2, &AccessWorker2::process);
    //End Thread :
    connect(accessWorker2, &AccessWorker2::finished, thread, &QThread::quit);
    connect(accessWorker2, &AccessWorker2::finished, accessWorker2, &AccessWorker2::deleteLater);

    thread->start();
}

void Bdd::registerAccess(int ErrorType)
{
    QThread *thread = new QThread;
    AccessWorker* accessWorker = new AccessWorker(ErrorType);
    accessWorker->moveToThread(thread);

    //Start Thread :
    connect(thread, &QThread::started, accessWorker, &AccessWorker::process);
    //End Thread :
    connect(accessWorker, &AccessWorker::finished, thread, &QThread::quit);
    connect(accessWorker, &AccessWorker::finished, accessWorker, &AccessWorker::deleteLater);

    thread->start();
}

void Bdd::updatePersonaByAcceso(Acceso *acceso)
{
    QSqlQuery query(QSqlDatabase::database("acceso"));

    qDebug() << "Update Data persona";

    QString sql = "UPDATE persona SET autorizado=:autorizado";
    sql += " WHERE rut=:rut and typeHash=0;";
    query.prepare(sql);

    int idAuth = acceso->idAuth();

    if(idAuth == Acceso::PERSON_OK)
        idAuth = Acceso::PERSON_SERVICE_USED;

    qDebug() << "Registro local grabado :";
    qDebug() << "state        : " << idAuth;

    query.bindValue(":autorizado", idAuth);
    query.bindValue(":rut", acceso->rut());

    if (!query.exec())
        qCritical() << "Query Error (updatePersonaByAcceso.update) : " << query.lastError();
}

void Bdd::updateCasinoService(Acceso *acceso) {

    QSqlQuery query(QSqlDatabase::database("acceso"));

    if(acceso->count_casino() > 0) {

        QString sql = "UPDATE service SET nb=:servicio WHERE type=:tipo";
        query.prepare(sql);

        query.bindValue(":servicio", QString::number(acceso->count_casino()));
        query.bindValue(":tipo", "normal");

        if (!query.exec())
            qCritical() << "Query Error (updateCasinoService.update) : " << query.lastError();
    }
}

int Bdd::casinoService() {

    QSqlQuery query(QSqlDatabase::database("acceso"));
    QString sql = "SELECT nb FROM service WHERE type='normal'";
    int result = -1;

    query.prepare(sql);

    if (!query.exec())
        qCritical() << "Query Error (updateCasinoService.update) : " << query.lastError();

    if(query.first())
        result = (query.value(0).toString() == "PD")? -1 :query.value(0).toInt();

    return result;
}

bool Bdd::checkDatabaseFile(const QString &basePath)
{
    QFile databaseFile(basePath);

    if (!databaseFile.exists())
    {
        qDebug() << "Database " << databaseFile.fileName() << " not exist";

        QString filename = basePath;
        QFile file(filename);
        file.open(QIODevice::WriteOnly);
        file.close();
        return true;
    }

    qDebug() << "Database " << databaseFile.fileName() << " exist";

    return false;
}

void Bdd::createDatabase(const QString &scriptPath, const QString &conn)
{
    QFile f(scriptPath);
    QSqlDatabase db = QSqlDatabase::database(conn);

    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << "Cannot open resource SQL script file";
        qCritical() << "Reason : " << f.errorString();
        qCritical() << "Path   : " << scriptPath;

        return;
    }

    QTextStream in(&f);
    QString line;
    QString sqlStatement = "";

    while (!in.atEnd()) {
        line = in.readLine();

        if(line.startsWith('#') || line.isEmpty())
            continue;
        sqlStatement += line;

        if(sqlStatement.trimmed().endsWith(";")){

            createTables(sqlStatement,db);
            sqlStatement = "";
        }
    }

}

void Bdd::createTables(const QString &sqlLine, QSqlDatabase db)
{
    QSqlQuery query(db);

    if(!query.exec(sqlLine))
        qCritical() << "Query Error (createTables) : " << query.lastError();
}

void Bdd::dataBaseChanged()
{
    QTimer::singleShot(3000, this, SLOT(saveDB()));
}

void Bdd::saveDB()
{
    this->saveDatabase("acceso","/var/lib/database/acceso.db");
    this->saveDatabase("config","/var/lib/database/config.db");
}

bool Bdd::saveDatabase(QString databaseName, QString fileName)
{
    qDebug() << "Save database " + databaseName + "...";

    QSqlQuery qry(QSqlDatabase::database(databaseName));

    bool state = false;
    qry.prepare("BEGIN IMMEDIATE;");
    if (!qry.exec())
        qCritical() << "Query Error (saveDatabase.begin) : " << qry.lastError();

    if(QFile::remove(fileName))
        state = QFile::copy("/usr/share/nginx/www/protected/data/" + databaseName + ".db", fileName);

    qry.prepare("ROLLBACK;");
    if (!qry.exec())
        qCritical() << "Query Error (saveDatabase.rollback) : " << qry.lastError();

    return state;
}
