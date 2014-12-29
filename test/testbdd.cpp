#include "testbdd.h"

void TestBdd::initTestCase()
{
    QString accesoFile = "/usr/share/nginx/www/protected/data/acceso.db";
    QString configFile = "/usr/share/nginx/www/protected/data/config.db";

    QFile accesoBDDFile(accesoFile);
    QFile configBDDFile(configFile);

    QVERIFY(accesoBDDFile.exists() == true);
    QVERIFY(configBDDFile.exists() == true);

    QSqlDatabase accesoBDD = QSqlDatabase::addDatabase("QSQLITE","acceso");
    QSqlDatabase configBDD = QSqlDatabase::addDatabase("QSQLITE","config");

    accesoBDD.setDatabaseName(accesoFile);
    configBDD.setDatabaseName(configFile);

    QCOMPARE(accesoBDD.open(),true);
    QCOMPARE(configBDD.open(),true);
}

void TestBdd::createPersona()
{
    Acceso acceso;
    acceso.setIdAuth(0);
    acceso.setName("Gilles Grandguillaume");
    acceso.setUuid("1058202051");
    acceso.setRut("23931696");
    acceso.setDv("4");

    Bdd::createPersona(acceso);

    // Verify RUT unique :
    QSqlQuery query(QSqlDatabase::database("acceso"));

    QString sql = "INSERT INTO persona(rut,dv,uuid) VALUES (:rut,:dv,:uuid);";
    query.prepare(sql);

    query.bindValue(":uuid", "1058202051");
    query.bindValue(":rut", "23931696");
    query.bindValue(":dv", "4");

    QCOMPARE(query.exec(),false);
}

void TestBdd::updatePersona()
{
    Acceso acceso;
    QSqlRecord record;
    Persona *persona = new Persona;

    persona->setRut("23931696");
    persona->setDv("4");
    persona->setUuid("1058202051");

    acceso.setIdAuth(0);
    acceso.setName("Gilles Grandguillaume");
    acceso.setUuid("1058202051");
    acceso.setRut("23931696");
    acceso.setDv("4");

    Bdd::updatePersonaByAcceso(acceso);
    record = Bdd::identificationCredencial(persona);
    QCOMPARE(record.value("autorizado").toInt(),0);

    acceso.setIdAuth(1);
    Bdd::updatePersonaByAcceso(acceso);
    record = Bdd::identificationCredencial(persona);
    QCOMPARE(record.value("autorizado").toInt(),1);

    acceso.setName("Gilles Andre Joseph Grandguillaume");
    Bdd::updatePersonaByAcceso(acceso);
    record = Bdd::identificationCredencial(persona);
    QVERIFY(record.value("nombre").toString() == "Gilles Andre Joseph Grandguillaume");
}

void TestBdd::identificationCredencial()
{
    Persona *persona = new Persona;

    persona->setUuid("1058202051");
    persona->setRut("23931696");
    persona->setDv("4");

    QSqlRecord record = Bdd::identificationCredencial(persona);

    QCOMPARE(record.value("autorizado").toInt(),1);
    QVERIFY(record.value("nombre").toString() == "Gilles Andre Joseph Grandguillaume");
}

void TestBdd::cleanupTestCase()
{
    QSqlQuery query(QSqlDatabase::database("acceso"));

    QString sql = "DELETE FROM persona WHERE uuid=:uuid AND rut=:rut;";
    query.prepare(sql);

    query.bindValue(":uuid", "1058202051");
    query.bindValue(":rut", "23931696");

    QCOMPARE(query.exec(),true);

    QSqlDatabase::database("acceso").close();
    QSqlDatabase::database("config").close();

    QSqlDatabase::removeDatabase("acceso");
    QSqlDatabase::removeDatabase("config");
}
