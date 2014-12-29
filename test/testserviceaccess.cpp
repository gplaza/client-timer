#include "testserviceaccess.h"

void TestServiceAccess::initTestCase()
{
    endPoint = "http://webservices.utfsm.cl/Ws_Asistencia_ControlAcceso/Ws_Principal?wsdl";
    soapAction = "http://principal/Ws_Principal/Modulo_AsistenciaRequest";
    usm = "80339";
    timeout = 25;
}

void TestServiceAccess::testOnlineAccessCard()
{
    if(!QSqlDatabase::database("acceso").isValid())
    {
        QString accesoFile = "/usr/share/nginx/www/protected/data/acceso.db";
        QSqlDatabase accesoBDD = QSqlDatabase::addDatabase("QSQLITE","acceso");
        accesoBDD.setDatabaseName(accesoFile);
        accesoBDD.open();
    }

    if(!QSqlDatabase::database("config").isValid())
    {
        QString configFile = "/usr/share/nginx/www/protected/data/config.db";
        QSqlDatabase configBDD = QSqlDatabase::addDatabase("QSQLITE","config");
        configBDD.setDatabaseName(configFile);
        configBDD.open();
    }

    if(!QSqlDatabase::database("syncro").isValid())
    {
        QSqlDatabase SyncroBDD = QSqlDatabase::addDatabase("QSQLITE","syncro");
        SyncroBDD.setDatabaseName(":memory:");
        SyncroBDD.open();
        Bdd bdd;
        bdd.createDatabase("/usr/share/nginx/www/protected/data/schema.syncro.sql","syncro");
    }

    SoapClient *soapClient = new SoapClient();
    ServiceAccess service(soapClient);
    Synchroniser sync(soapClient);

    QObject::connect(&service, &ServiceAccess::synchroniseOffLine, &sync, &Synchroniser::offLine);
    QObject::connect(&service, &ServiceAccess::synchroniseOnLine, &sync, &Synchroniser::onLine);

    Persona *pers1;

    pers1 = new Persona;
    pers1->setUuid("2676021325");
    pers1->setRut("11622337");
    pers1->setDv("6");
    pers1->setTipoMarca(2);
    service.setTypeEvent(ServiceAccess::ACCESS_CARD);
    service.check(pers1);

    delete pers1;
    delete soapClient;
}

void TestServiceAccess::testOfflineAccessCard()
{
    endPoint = "http://dummy.utfsm.cl/Ws_Asistencia_ControlAcceso/Ws_Principal?wsdl";
    soapAction = "http://principal/Ws_Principal/Modulo_AsistenciaRequest";
    usm = "A0001";
    timeout = 25;

    TestServiceAccess::testOnlineAccessCard();
}


void TestServiceAccess::testSynchroniseAccessCard()
{
    endPoint = "http://webservices.utfsm.cl/Ws_Asistencia_ControlAcceso/Ws_Principal?wsdl";
    soapAction = "http://principal/Ws_Principal/Modulo_AsistenciaRequest";
    usm = "A0001";
    timeout = 25;

    TestServiceAccess::testOnlineAccessCard();
}

void TestServiceAccess::cleanupTestCase()
{
    QSqlDatabase::database("config").close();
    QSqlDatabase::database("acceso").close();
    QSqlDatabase::database("syncro").close();

    QSqlDatabase::removeDatabase("config");
    QSqlDatabase::removeDatabase("acceso");
    QSqlDatabase::removeDatabase("syncro");
}


