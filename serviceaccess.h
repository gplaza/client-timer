#ifndef SERVICEACCESS_H
#define SERVICEACCESS_H
#include <QtCore>
#include <QTcpSocket>
#include <Logger.h>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <persona.h>
#include <soapclient.h>
#include <acceso.h>
#include <bdd.h>
#include <buzzer.h>
#include <QScopedPointer>

class ServiceAccess : public QObject
{
    Q_OBJECT
public:
    ServiceAccess(SoapClient *soapClient, Acceso *acceso, QObject *parent = 0);

signals:
    void offLine();
    void onLine();
    void finished();
    void synchroniseOnLine(Acceso *acceso, Persona &persona);
    void synchroniseOffLine(Acceso *acceso, Persona &persona);
    void hashResponse(Acceso &acceso);
    void fotoChanged();
    void changeStatus();

public slots:
    void check(const QString id);

private:
    void finalizeResponse();
    void on_online();
    void on_offline();
    QScopedPointer<SoapClient> soapClient;
    Acceso *acceso;
    Persona persona;
};

#endif // SERVICEACCESS_H
