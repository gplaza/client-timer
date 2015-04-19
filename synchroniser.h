#ifndef SYNCHRONISER_H
#define SYNCHRONISER_H
#include <QDateTime>
#include <QTextStream>
#include <QDebug>

#include <bdd.h>
#include <acceso.h>
#include <soapclient.h>

class SynchroWorker : public QObject
{
    Q_OBJECT
public:
    SynchroWorker(SoapClient *soapClient)
    {
        this->soapClient = soapClient;
        this->soapClient->init();
    }

    ~SynchroWorker()
    {
        qDebug() << "Desctructor SynchroWorker";
    }

public slots:
    void process()
    {
        Persona persona;
        QDateTime date = Bdd::syncAccess(persona);

        while(!date.isNull())
        {
            Acceso acceso;
            bool result = soapClient->syncro(&persona,acceso,date);

            if(!result)
                break;

            qDebug() << acceso.count_casino();
            qDebug() << acceso.count_lunch();
            qDebug() << acceso.count_dinner();

            Bdd::deleteAccess(persona, date);
            date = Bdd::syncAccess(persona);
        }

        emit finished();
    }

signals:
    void finished();
    void error(QString err);

private:
    SoapClient *soapClient;

};

class Synchroniser : public QObject
{
    Q_OBJECT
public:
    Synchroniser(SoapClient *soapClient, QObject *parent = 0);

private:
    SoapClient *soapClient;

public slots:
    void onLine(Acceso &acceso, Persona &persona);
    void offLine(Acceso &acceso, Persona &persona);

};

#endif // SYNCHRONISER_H
