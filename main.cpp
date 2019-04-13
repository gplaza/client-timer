#include <QCoreApplication>
#include <QDebug>
#include <QSettings>
#include <ConsoleAppender.h>
#include <protector.h>
#include <credencial.h>
#include <Logger.h>
#include <serviceaccess.h>
#include <fingerprint.h>
#include <bdd.h>
#include <door.h>
#include <configurator.h>
#include <configuratoradapter.h>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <qnetworkaccessmanager.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    wiringPiSetup();

    ConsoleAppender* consoleAppender = new ConsoleAppender();
    consoleAppender->setFormat("[%-7l] %t{dd/MM/yyyy HH:mm:ss} %m\n");
    consoleAppender->setDetailsLevel("debug");
    Logger::registerAppender(consoleAppender);

    ConfiguratorAdapter *configAdapt = new ConfiguratorAdapter();
    Configurator::instance()->init("/usr/share/nginx/www/protected/data/config.db");

    QSet<QString> confs;
    confs << "timeout";
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

    //Init Hour

    QNetworkRequest timeRequest(QUrl("http://158.170.10.15/access-manager/api/system/time"));

    // HTTP Basic authentication header value: base64(username:password)
    QString username = "admin"; //config["usernameApiServer"];
    QString password = "admin"; //config["passwordApiServer"];

    QString concatenated = username + ":" + password;
    QByteArray dataLogin = concatenated.toLocal8Bit().toBase64();
    QString headerDataAccessManager = "Basic " + dataLogin;
    QTimer *timeout = new QTimer();

    timeRequest.setRawHeader("Authorization", headerDataAccessManager.toLocal8Bit());
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QEventLoop loopWait;

    QObject::connect(timeout, &QTimer::timeout, &loopWait, &QEventLoop::quit);
    QNetworkReply *timeReply = manager->get(timeRequest);
    QObject::connect(timeReply, &QNetworkReply::finished, &loopWait, &QEventLoop::quit);

    // 4s timeout for machine API
    timeout->start(4000);
    loopWait.exec();

    if(timeout->isActive()){

        QByteArray timeResponse = timeReply->readAll();

        QJsonDocument timejson = QJsonDocument::fromJson(timeResponse);
        QJsonObject qJsonObject = timejson.object();
        QString dateJson =  qJsonObject["time"].toString();

        QString command = "date -s '" + dateJson + "'";
        popen(command.toStdString().c_str(),"r");

        qDebug() << "Set Current DateTime from server : ";
        qDebug() << dateJson;

        timeout->stop();

    } else {

        qFatal() << "Error timeout API date Access Manager";
    }

    //Init DAL;
    Bdd bdd;
    bdd.openDatabase();
    Acceso *acceso = new Acceso();

    //Init Web service component
    ServiceAccess serviceAccess(acceso);

    //Physical Access object :
    Door door(4);
    Credencial credencial("pn532_spi:/dev/spidev0.0:500000");
    Fingerprint fingerprint("/dev/ttyAMA0",7);

    QObject::connect(&serviceAccess, &ServiceAccess::verifFingerprint, &fingerprint, &Fingerprint::verifFingerprint);

    // Data ready on device
    // ** data on rfid **
    QObject::connect(&credencial, &Credencial::dataReady, &credencial, &Credencial::stopWaitForTag);
    QObject::connect(&credencial, &Credencial::dataReady, &serviceAccess, &ServiceAccess::check);

    // Verif Fingerprint
    QObject::connect(&fingerprint, &Fingerprint::compareResult, &serviceAccess, &ServiceAccess::checkfingerprint);

    // End process :
    QObject::connect(&serviceAccess, &ServiceAccess::openDoor, &door, &Door::openDoor);
    QObject::connect(&serviceAccess, &ServiceAccess::finished, &credencial, &Credencial::waitForTag);

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
