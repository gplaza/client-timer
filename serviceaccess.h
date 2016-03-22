#ifndef SERVICEACCESS_H
#define SERVICEACCESS_H
#include <QtCore>
#include <Logger.h>
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
    void finished();
    void verifFingerprint(int userID);
    void hashResponse(Acceso &acceso);
    void changeStatus();

public slots:
    void check(const QString id);
    void finalizeResponse();

private:
    bool checkMachineRules();
    Acceso *acceso;
};

#endif // SERVICEACCESS_H
