#ifndef SERVICEACCESS_H
#define SERVICEACCESS_H
#include <QtCore>
#include <QTcpSocket>
#include <Logger.h>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <persona.h>
#include <acceso.h>
#include <bdd.h>
#include <buzzer.h>
#include <QScopedPointer>

class ServiceAccess : public QObject
{
    Q_OBJECT
public:
    ServiceAccess(Acceso *acceso, QObject *parent = 0);

signals:
    void openDoor();
    void offLine();
    void finished();
    void verifFingerprint(int userID);
    void synchroniseOnLine(Acceso *acceso, Persona &persona);
    void synchroniseOffLine(Acceso *acceso, Persona &persona);
    void hashResponse(Acceso &acceso);
    void changeStatus();

public slots:
    void check(const QString id);
    void on_offline();

private:
    void finalizeResponse();
    void on_online();
    Acceso *acceso;
    Persona persona;
};

#endif // SERVICEACCESS_H
