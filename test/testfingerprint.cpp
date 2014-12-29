#include "testfingerprint.h"

void TestFingerprint::initTestCase()
{

}

void TestFingerprint::testRegisterNewUser()
{

    if(!QSqlDatabase::database("acceso").isValid())
    {
        QString accesoFile = "/usr/share/nginx/www/protected/data/acceso.db";
        QSqlDatabase accesoBDD = QSqlDatabase::addDatabase("QSQLITE","acceso");
        accesoBDD.setDatabaseName(accesoFile);
        accesoBDD.open();
    }

    if(!QSqlDatabase::database("syncro").isValid())
    {
        QSqlDatabase SyncroBDD = QSqlDatabase::addDatabase("QSQLITE","syncro");
        SyncroBDD.setDatabaseName(":memory:");
        SyncroBDD.open();
        Bdd bdd;
        bdd.createDatabase("/usr/share/nginx/www/protected/data/schema.syncro.sql","syncro");
    }

    QString endPoint = "http://ssodes.usm.cl/Ws_Asistencia_ControlAcceso/Ws_Principal?wsdl";
    QString soapAction = "http://principal/Ws_Principal/Modulo_AsistenciaRequest";
    QString usm = "A0001";
    int timeout = 200;

    Screen screen;
    SoapClient *soapClient = new SoapClient();
    ServiceAccess *service = new ServiceAccess(soapClient);
    Fingerprint *fingerprint = new Fingerprint("/dev/ttyAMA0");
    Synchroniser *sync = new Synchroniser(soapClient);

    userID = fingerprint->getuserIDavailable();

    QObject::connect(fingerprint, &Fingerprint::setTypeEvent, service, &ServiceAccess::setTypeEvent);

    //QObject::connect(service, &ServiceAccess::synchroniseOffLine, this, &TestFingerprint::stubSynchroniserOffLine);
    //QObject::connect(service, &ServiceAccess::synchroniseOnLine, this, &TestFingerprint::stubSynchroniserOnLine);

    QObject::connect(service, &ServiceAccess::synchroniseOffLine, sync, &Synchroniser::offLine);
    QObject::connect(service, &ServiceAccess::synchroniseOnLine, sync, &Synchroniser::onLine);

    //QObject::connect(service, &ServiceAccess::hashResponse, fingerprint, &Fingerprint::registerNewUser);
    //QObject::connect(fingerprint, &Fingerprint::sendToScreen, &screen, &Screen::showMessage);
    //QObject::connect(fingerprint, &Fingerprint::sendCautionToScreen, &screen, &Screen::showWaitMessage);
    //QObject::connect(fingerprint, &Fingerprint::endReadFingerprint, service, &ServiceAccess::check);

    Persona *persona = new Persona;
    persona->setUuid("42779729");
    persona->setRut("16813180");
    persona->setDv("1");
    service->setTypeEvent(ServiceAccess::INFO_FINGERPRINT);
    service->check(persona);

    persona->deleteLater();
    service->deleteLater();
    delete fingerprint;
    delete soapClient;

// Credencial : computershop
//uid : 3769515966
//rut : 11111111
//dv : 1

// Credencial : test
//64323478
//44444444
//4


}

void TestFingerprint::getHashUser()
{
    Fingerprint *fingerprint = new Fingerprint("/dev/ttyAMA0");
    QString hash;
    fingerprint->getHashUser(2,hash);

    qDebug() << "Hash : " << hash;

    delete fingerprint;
}

void TestFingerprint::testDeleteUser()
{
    Fingerprint *fingerprint = new Fingerprint("/dev/ttyAMA0");
    delete fingerprint;
}

void TestFingerprint::stubSynchroniserOnLine(Acceso &acceso, Persona &persona)
{
    qDebug() << "Stub syncroniser Online ...";
    qDebug() << acceso.objectName();
    qDebug() << persona.objectName();
}

void TestFingerprint::stubSynchroniserOffLine(Acceso &acceso, Persona &persona)
{
    qDebug() << "Stub syncroniser Offline ...";
    qDebug() << acceso.objectName();
    qDebug() << persona.objectName();
}
