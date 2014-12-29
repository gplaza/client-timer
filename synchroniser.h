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
    }

    ~SynchroWorker()
    {
        qDebug() << "Desctructor SynchroWorker";
    }

public slots:
    void process()
    {
        Acceso *acceso = new Acceso();
        acceso = Bdd::syncAccess();

        while(!acceso->uuid().isEmpty())
        {
            soapClient->action(acceso);

            if (acceso->idAuth() == -1)
                break;

            Bdd::deleteAccess(acceso);
            acceso = Bdd::syncAccess();
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
