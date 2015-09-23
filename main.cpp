#include <QCoreApplication>
#include <QDebug>
#include <QSettings>
#include <ConsoleAppender.h>
#include <protector.h>
#include <synchroniser.h>
#include <credencial.h>
#include <Logger.h>
#include <serviceaccess.h>
#include <fingerprint.h>
#include <bdd.h>
#include <configurator.h>
#include <configuratoradapter.h>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    ConsoleAppender* consoleAppender = new ConsoleAppender();
    consoleAppender->setFormat("[%-7l] %t{dd/MM/yyyy HH:mm:ss} %m\n");
    consoleAppender->setDetailsLevel("debug");
    Logger::registerAppender(consoleAppender);

    ConfiguratorAdapter *configAdapt = new ConfiguratorAdapter();
    Configurator::instance()->init("/usr/share/nginx/www/protected/data/config.db");

    QSet<QString> confs;
    confs << "timeout";
    confs << "timeShowUser";
    confs << "casinoName";
    confs << "emailPrinterError";
    confs << "endPointCasino";
    confs << "soapActionCasinoValidar";
    confs << "soapActionCasinoTransaction";
    confs << "endPointFoto";
    confs << "soapActionFoto";
    confs << "usm";
    confs << "serial";
    confs << "logLevel";
    confs << "logFormat";

    QMap<QString,QString> config = Configurator::instance()->getConfigs(confs);
    consoleAppender->setDetailsLevel(config["logLevel"]);

    qDebug() << "Init parameters : ";
    Configurator::instance()->getCacheConfigToDebug();
    Protector licence(config["serial"]);

    if(!licence.isValid())
    {
        qCritical() << "Licence key invalid ...";
        QCoreApplication::exit(1);
        return 1;
    }

    //Init DAL;
    Bdd bdd;
    bdd.openDatabase();
    QString casinoName = config["casinoName"];
    Acceso *acceso = new Acceso(casinoName);

    //Init Web service component
    ServiceAccess serviceAccess(acceso);
    Synchroniser sync();

    //Physical Access object :
    Credencial credencial("pn532_spi:/dev/spidev0.0:500000");
    Fingerprint fingerprint("/dev/ttyAMA0");

    // Manage fingerPrintReader
    // ** detected **
    QObject::connect(&fingerprint, &Fingerprint::fingerDetected, &fingerprint, &Fingerprint::processDataFingerprint);
    // ** detected unknown **
    QObject::connect(&fingerprint, &Fingerprint::unknownFinger, &fingerprint, &Fingerprint::waitForFinger);

    // Data ready on device
    // ** data on rfid **
    QObject::connect(&credencial, &Credencial::dataReady, &fingerprint, &Fingerprint::stopWaitForFinger);
    QObject::connect(&credencial, &Credencial::dataReady, &credencial, &Credencial::stopWaitForTag);
    QObject::connect(&credencial, &Credencial::dataReady, &serviceAccess, &ServiceAccess::check);
    // ** data on fingerPrintReader **
    QObject::connect(&fingerprint, &Fingerprint::dataReady, &fingerprint, &Fingerprint::stopWaitForFinger);
    QObject::connect(&fingerprint, &Fingerprint::dataReady, &credencial, &Credencial::stopWaitForTag);
    QObject::connect(&fingerprint, &Fingerprint::dataReady, &serviceAccess, &ServiceAccess::check);

    // End process :
    QObject::connect(&serviceAccess, &ServiceAccess::finished, &credencial, &Credencial::waitForTag);
    QObject::connect(&serviceAccess, &ServiceAccess::finished, &fingerprint, &Fingerprint::waitForFinger);

    // Syncro process
    // QObject::connect(&serviceAccess, &ServiceAccess::synchroniseOffLine, &sync, &Synchroniser::offLine);
    // QObject::connect(&serviceAccess, &ServiceAccess::synchroniseOnLine, &sync, &Synchroniser::onLine);
    // QObject::connect(&sync, &Synchroniser::registerFingerPrint, &fingerprint, &Fingerprint::registerNewUser);

    // Adapter connection
    QObject::connect(configAdapt,&ConfiguratorAdapter::deleteFingerPrint,&fingerprint,&Fingerprint::externDeleteUser);
    QObject::connect(configAdapt,&ConfiguratorAdapter::registerFingerPrint,&fingerprint,&Fingerprint::externInsertUser);
    QObject::connect(configAdapt,&ConfiguratorAdapter::updateFingerPrint,&fingerprint,&Fingerprint::externUpdateUser);
    QObject::connect(&fingerprint,&Fingerprint::responseRegister,configAdapt,&ConfiguratorAdapter::response);

    // Save connection
    QObject::connect(Configurator::instance(),&Configurator::configurationChanged,&bdd,&Bdd::dataBaseChanged);
    QObject::connect(configAdapt,&ConfiguratorAdapter::userChanged,&bdd,&Bdd::dataBaseChanged);

    return app.exec();
}
