
#include "bdd.h"

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

QSqlRecord Bdd::identificationCredencial(Persona *persona)
{
    QSqlDatabase db = QSqlDatabase::database("acceso");
    QSqlRecord result;
    QSqlQuery query(db);

    QString sql = "SELECT autorizado,nombre FROM persona WHERE rut=:rut and dv=:dv";

    if(persona->tipoMarca() == 1)
        sql += " and hash=:hash";
    if(persona->tipoMarca() == 2)
        sql += " and uuid=:uuid";

    query.prepare(sql);

    if(persona->tipoMarca() == 1)
        query.bindValue(":hash", persona->uuid());
    if(persona->tipoMarca() == 2)
        query.bindValue(":uuid", persona->uuid());

    query.bindValue(":rut", persona->rut());
    query.bindValue(":dv", persona->dv());

    if (!query.exec())
        qCritical() << "Query Error (identificationCredencial) : " << query.lastError();

    if(query.first())
        result = query.record();

    return result;
}

QSqlRecord Bdd::identificationFingerprint(int id)
{
    QSqlQuery query(QSqlDatabase::database("acceso"));
    QSqlRecord result;

    QString sql = "SELECT hash,rut,dv FROM persona WHERE id_huella=:id_huella";

    query.prepare(sql);
    query.bindValue(":id_huella", id);

    if (!query.exec())
        qCritical() << "Query Error (identificationFingerprint) : " << query.lastError();

    if(query.first())
    {
        result = query.record();
    }

    return result;
}

QString Bdd::textAuthentication(Acceso &acceso)
{
    QSqlQuery query(QSqlDatabase::database("config"));
    QString result = "";

    QString sql = "SELECT descripcion FROM estado WHERE valor=:valor";

    query.prepare(sql);
    query.bindValue(":valor", acceso.idAuth());

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

void Bdd::saveAccess(Acceso &acceso, Persona &persona)
{
    QSqlQuery query(QSqlDatabase::database("syncro"));

    QString sql = "INSERT INTO acceso(uuid, dispo, tipoMarca, fecha) VALUES (:uuid, :dispo, :tipoMarca, :fecha);";
    query.prepare(sql);

    query.bindValue(":uuid", persona.uuid());
    query.bindValue(":dispo", Configurator::instance()->getConfig("usm"));
    query.bindValue(":fecha", acceso.dateFormated("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":tipoMarca", persona.tipoMarca());

    if (!query.exec())
        qCritical() << "Query Error (SaveAccess) : " << query.lastError();
}

void Bdd::deleteAccess(Acceso *acceso)
{
    QSqlQuery query(QSqlDatabase::database("syncro"));

    QString sql = "DELETE FROM acceso WHERE fecha=:fecha AND uuid=:uuid AND rut=:rut AND dv=:dv;";
    query.prepare(sql);

    query.bindValue(":fecha", acceso->dateFormated("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":uuid", acceso->uuid());
    query.bindValue(":rut", acceso->rut());
    query.bindValue(":dv", acceso->dv());

    if (!query.exec())
        qCritical() << "Query Error (deleteAccess) : " << query.lastError();
}

bool Bdd::checkPersona(Persona &persona)
{
    QSqlQuery query(QSqlDatabase::database("acceso"));

    QString sql = "SELECT rut,dv,uuid,id_huella FROM persona WHERE uuid=:uuid";
    query.prepare(sql);
    query.bindValue(":uuid", persona.uuid());

    if (!query.exec())
        qCritical() << "Query Error (checkPersonaByUuid) : " << query.lastError();

    if (query.first())
    {
        persona.setRut(query.value(0).toString());
        persona.setDv(query.value(1).toString());
        persona.setUuid(query.value(2).toString());
        persona.setFingerprintID(query.value(3).toInt());

        return true;
    }

    return false;
}

void Bdd::createPersona(Acceso &acceso)
{
    QSqlQuery query(QSqlDatabase::database("acceso"));

    QString sql = "INSERT INTO persona(autorizado,nombre,rut,dv,uuid) VALUES (:autorizado,:nombre,:rut,:dv,:uuid);";
    query.prepare(sql);

    query.bindValue(":autorizado", acceso.idAuth());
    query.bindValue(":nombre", acceso.name());
    query.bindValue(":uuid", acceso.uuid());
    query.bindValue(":rut", acceso.rut());
    query.bindValue(":dv", acceso.dv());

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

void Bdd::updatePersonaByAcceso(Acceso &acceso)
{
    QSqlQuery query(QSqlDatabase::database("acceso"));

    // Select current user for diff
    QString sql = "SELECT uuid,autorizado,nombre FROM persona WHERE rut=:rut";
    query.prepare(sql);
    query.bindValue(":rut", acceso.rut());

    if (!query.exec())
        qCritical() << "Query Error (updatePersonaByAcceso.select) : " << query.lastError();

    if (query.first())
    {
        bool update = false;
        bool updateUUID = !acceso.uuid().isEmpty() && acceso.uuid().length() <= 10;

        if(updateUUID)
            update = acceso.uuid() != query.value(0).toString();
        if(!update)
            update = acceso.idAuth() != query.value(1).toInt();
        if(!update)
            update = acceso.name() != query.value(2).toString();

        if(update)
        {
            qDebug() << "Update Data persona";

            sql = "UPDATE persona SET autorizado=:autorizado, nombre=:nombre";
            sql += updateUUID? ", uuid=:uuid" : "";
            sql += " WHERE rut=:rut;";
            query.prepare(sql);

            if(updateUUID)
                query.bindValue(":uuid", acceso.uuid());

            query.bindValue(":autorizado", acceso.idAuth());
            query.bindValue(":nombre", acceso.name());
            query.bindValue(":rut", acceso.rut());

            if (!query.exec())
                qCritical() << "Query Error (updatePersonaByAcceso.update) : " << query.lastError();
        }
    }
}

Acceso* Bdd::syncAccess()
{
    QSqlQuery query(QSqlDatabase::database("syncro"));

    if (!query.exec("SELECT fecha, tipoMarca, uuid, rut, dv FROM acceso ORDER BY fecha LIMIT 1"))
        qCritical() << "Query Error (checkOfflineData) : " << query.lastError();

    Acceso *acceso = new Acceso;

    if(query.first())
    {
        QSqlRecord access = query.record();

        QDateTime date = QDateTime::fromString(access.value("fecha").toString(),"yyyy-MM-dd hh:mm:ss");
        QString tipoMarca = access.value("tipoMarca").toString();
        QString uuid = access.value("uuid").toString();
        QString rut = access.value("rut").toString();
        QString dv = access.value("dv").toString();

        acceso->setDate(date);
        acceso->setUuid(uuid);
        acceso->setRut(rut);
        acceso->setDv(dv);
    }

    return acceso;
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

