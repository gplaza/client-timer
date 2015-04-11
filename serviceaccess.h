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
#include <printer.h>

class ServiceAccess : public QObject
{
    Q_OBJECT

public:
    ServiceAccess(SoapClient *soapClient, QObject *objectView, QObject *parent = 0);

signals:
    void sendToScreen(const QString &m);
    void offLine();
    void onLine();
    void finished();
    void synchroniseOnLine(Acceso &acceso, Persona &persona);
    void synchroniseOffLine(Acceso &acceso, Persona &persona);
    void hashResponse(Acceso &acceso);

public slots:
    void check(QString &id);
    void initUi();

private:
    void finalizeResponse(Acceso &acceso);
    void on_online();
    void on_offline();
    SoapClient *soapClient;
    QObject *objectView;
    QTcpSocket service;
    Persona persona;
};

#endif // SERVICEACCESS_H
