#include <QCoreApplication>
#include <QDebug>
#include <QSettings>
#include <ConsoleAppender.h>
#include <QtQuick>
#include <QtGui>

#include <protector.h>
#include <synchroniser.h>
#include <soapclient.h>
#include <credencial.h>
#include <screen.h>
#include <Logger.h>
#include <serviceaccess.h>
#include <fingerprint.h>
#include <bdd.h>
#include <configurator.h>

#include <stdsoap2.h>

//#include <QtTest/QTest>
//#include <test/testbdd.h>
//#include <test/testserviceaccess.h>
//#include <test/testfingerprint.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQuickView view;

    app.setOverrideCursor(Qt::BlankCursor);
    view.setResizeMode(QQuickView::SizeRootObjectToView);

    ConsoleAppender* consoleAppender = new ConsoleAppender();
    consoleAppender->setFormat("[%-7l] %t{dd/MM/yyyy HH:mm:ss} %m\n");
    consoleAppender->setDetailsLevel("debug");
    Logger::registerAppender(consoleAppender);

    Configurator::instance()->setDB("/usr/share/nginx/www/protected/data/config.db",true);

    QSet<QString> confs;
    confs << "timeout";
    confs << "endPointCasino";
    confs << "soapActionCasinoValidar";
    confs << "soapActionCasinoTransaction";
    confs << "usm";
    confs << "serial";
    confs << "logLevel";
    confs << "logFormat";

    QMap<QString,QString> config = Configurator::instance()->getConfigs(confs);

    consoleAppender->setDetailsLevel(config["logLevel"]);

    qDebug() << "Init parameters : ";

    QMapIterator<QString,QString> i(config);
    while (i.hasNext()) {
        i.next();
        qDebug() << i.key().leftJustified(30,' ') + " : " << i.value();
    }

    view.setSource(QUrl(QCoreApplication::applicationDirPath() + "/qml/main.qml"));
    view.show();

    QObject *objectView = view.rootObject();


    Protector licence(config["serial"]);

    if(!licence.isValid())
    {
        qCritical() << "Licence key invalid ...";
        QCoreApplication::exit(1);
        return 1;
    }

    SoapClient *soapClient = new SoapClient();
    ServiceAccess serviceAccess(soapClient,objectView);
    Synchroniser sync(soapClient);
    Screen display;
    Bdd bdd;

    //Init database;
    bdd.openDatabase();

    //Physical Access object :
    Credencial credencial("pn532_spi:/dev/spidev0.0:500000");
    // Fingerprint fingerprint("/dev/ttyAMA0");

    //QObject::connect(&fingerprint, &Fingerprint::setTypeEvent, &serviceAccess, &ServiceAccess::setTypeEvent);

    // Manage fingerPrintReader
    //QObject::connect(&fingerprint, &Fingerprint::fingerDetected, &fingerprint, &Fingerprint::processDataFingerprint);

    // Manage screen message
    QObject::connect(&credencial, &Credencial::sendToScreen, &display, &Screen::showMessage);
    QObject::connect(&serviceAccess, &ServiceAccess::sendToScreen, &display, &Screen::showMessage);
    //QObject::connect(&fingerprint, &Fingerprint::sendToScreen, &display, &Screen::showMessage);

    // End device reading
    QObject::connect(&credencial, &Credencial::endReadRFID, &serviceAccess, &ServiceAccess::check);
    //QObject::connect(&fingerprint, &Fingerprint::endReadFingerprint, &serviceAccess, &ServiceAccess::check);
    //QObject::connect(&fingerprint, &Fingerprint::finished, &credencial, &Credencial::WaitForTag);
    //QObject::connect(&fingerprint, &Fingerprint::finished, &screen, &Screen::showTime);

    // Syncro process
    QObject::connect(&serviceAccess, &ServiceAccess::onLine, &display, &Screen::onLine);
    QObject::connect(&serviceAccess, &ServiceAccess::offLine, &display, &Screen::offLine);
    QObject::connect(&serviceAccess, &ServiceAccess::synchroniseOffLine, &sync, &Synchroniser::offLine);
    QObject::connect(&serviceAccess, &ServiceAccess::synchroniseOnLine, &sync, &Synchroniser::onLine);

    // End reading web service response
    QObject::connect(&serviceAccess, &ServiceAccess::finished, &credencial, &Credencial::WaitForTag);



    return app.exec();

    /*
    QStringList cmdlineArgs = QCoreApplication::arguments();

    if(cmdlineArgs.size() == 2 && cmdlineArgs[1].startsWith("--test"))
    {
        QString param = cmdlineArgs[1];

        if (param.contains("bdd") || param.contains("all"))
        {
            TestBdd testBdd;
            QTest::qExec(&testBdd);
        }
        if (param.contains("service") || param.contains("all"))
        {
            TestServiceAccess testService;
            QTest::qExec(&testService);
        }
        if (param.contains("finger") || param.contains("all"))
        {
            TestFingerprint testFingerprint;
            QTest::qExec(&testFingerprint);
        }

        QCoreApplication::exit(0);
        return 0;
    }
    */

    /*






















    view.setSource(QUrl(QCoreApplication::applicationDirPath() + "/qml/main.qml"));

    view.show();
    return app.exec();
    */
}
