
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

QSqlRecord Bdd::identificationCredencial(QString uuid)
{
    QSqlDatabase db = QSqlDatabase::database("acceso");
    QSqlRecord result;
    QSqlQuery query(db);

    QString sql = "SELECT rut,nombre,autorizado,id_huella,image FROM persona WHERE uuid=:uuid";

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

    QString sql = "SELECT hash, rut, nombre FROM persona WHERE id_huella=:id_huella";

    query.prepare(sql);
    query.bindValue(":id_huella", id);

    if (!query.exec())
        qCritical() << "Query Error (identificationFingerprint) : " << query.lastError();

    if(query.first())
        result = query.record();

    return result;
}

QSqlRecord Bdd::identificationOffline(QString rut)
{
    QSqlQuery query(QSqlDatabase::database("acceso"));
    QSqlRecord result;

    QString sql = "SELECT autorizado,nombre,count_lunch,count_dinner FROM persona WHERE rut=:rut";

    query.prepare(sql);
    query.bindValue(":rut", rut);

    if (!query.exec())
        qCritical() << "Query Error (identificationOffline) : " << query.lastError();

    if(query.first())
        result = query.record();

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

void Bdd::setImage(QString rut, QByteArray img)
{
    QSqlQuery query(QSqlDatabase::database("acceso"));

    QString sql = "UPDATE persona SET image=:image WHERE rut=:rut;";
    query.prepare(sql);

    query.bindValue(":image", img);
    query.bindValue(":rut", rut);

    if (!query.exec())
        qCritical() << "Query Error (setImage) : " << query.lastError();
}

QByteArray Bdd::getImage(QString rut)
{
    QSqlDatabase db = QSqlDatabase::database("acceso");
    QByteArray result;
    QSqlQuery query(db);

    QString sql = "SELECT image FROM persona WHERE rut=:rut";
    query.prepare(sql);
    query.bindValue(":rut", rut);

    if (!query.exec())
        qCritical() << "Query Error (getImage) : " << query.lastError();

    if(query.first())
        result = query.record().value("image").toByteArray();

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

    QString sql = "";

    if(persona.tipoMarca() == Persona::MARCA_FINGER)
        sql += "INSERT INTO acceso(uuid, rut, tipoMarca, fecha) VALUES (:uuid, :rut, :tipoMarca, :fecha);";
    if(persona.tipoMarca() == Persona::MARCA_RFID)
        sql += "INSERT INTO acceso(uuid, tipoMarca, fecha) VALUES (:uuid, :tipoMarca, :fecha);";

    query.prepare(sql);

    query.bindValue(":uuid", persona.uuid());
    query.bindValue(":fecha", acceso.dateFormated("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":tipoMarca", persona.tipoMarca());

    if(persona.tipoMarca() == Persona::MARCA_FINGER)
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

bool Bdd::checkPersona(Acceso &acceso)
{
    QSqlQuery query(QSqlDatabase::database("acceso"));

    QString sql = "SELECT uuid,id_huella FROM persona WHERE rut=:rut";
    query.prepare(sql);
    query.bindValue(":rut", acceso.rut());

    if (!query.exec())
        qCritical() << "Query Error (checkPersona) : " << query.lastError();

    return query.first();
}

void Bdd::createPersona(Acceso &acceso)
{
    QSqlQuery query(QSqlDatabase::database("acceso"));

    QString sql = "INSERT INTO persona(autorizado,nombre,rut,uuid) VALUES (:autorizado,:nombre,:rut,:uuid);";
    query.prepare(sql);

    query.bindValue(":autorizado", acceso.idAuth());
    query.bindValue(":nombre", acceso.name());
    query.bindValue(":uuid", acceso.uuid());
    query.bindValue(":rut", acceso.rut());

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

    bool updateUUID = !acceso.uuid().isEmpty() && acceso.uuid().length() <= 10;

    qDebug() << "Update Data persona";

    QString sql = "UPDATE persona SET autorizado=:autorizado, count_lunch=:count_lunch, count_dinner=:count_dinner";
    sql += updateUUID? ", uuid=:uuid" : "";
    sql += " WHERE rut=:rut;";
    query.prepare(sql);

    if(updateUUID)
        query.bindValue(":uuid", acceso.uuid());

    int idAuth = acceso.idAuth();
    int count_lunch = acceso.count_lunch();
    int count_dinner = acceso.count_dinner();

    if(idAuth == Acceso::PERSON_OK) {

        QString format = "HH:mm";
        QTime current = QTime::currentTime();
        QTime startLunch = QTime::fromString(Configurator::instance()->getConfig("startLunch"),format);
        QTime endLunch = QTime::fromString(Configurator::instance()->getConfig("endLunch"),format);
        QTime startDinner = QTime::fromString(Configurator::instance()->getConfig("startDinner"),format);
        QTime endDinner = QTime::fromString(Configurator::instance()->getConfig("endDinner"),format);

        if(startLunch <= current && current <= endLunch)
            count_lunch--;
        if(startDinner <= current && current <= endDinner)
            count_dinner--;

        idAuth = Acceso::PERSON_SERVICE_USED;

        qDebug() << "Registro local grabado :";
        qDebug() << "state        : " << idAuth;
        qDebug() << "count_lunch  : " << count_lunch;
        qDebug() << "count_dinner : " << count_dinner;
    }

    query.bindValue(":autorizado", idAuth);
    query.bindValue(":count_lunch", count_lunch);
    query.bindValue(":count_dinner", count_dinner);

    query.bindValue(":rut", acceso.rut());

    if (!query.exec())
        qCritical() << "Query Error (updatePersonaByAcceso.update) : " << query.lastError();
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

