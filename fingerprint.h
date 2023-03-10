#ifndef FINGERPRINT_H
#define FINGERPRINT_H

#include <persona.h>
#include <piezo.h>
#include <secugen_sda04.h>
#include <serviceaccess.h>
#include <bdd.h>

class Fingerprint : public SecugenSda04
{
    Q_OBJECT
public:
    Fingerprint(QString serialPort);
private:
    QTimer *cancelerTimer;

public slots:
    void checkFingerTouch();
    void processDataFingerprint();
    // -- function for API --
    void externInsertUser(QString &hash);
    void externDeleteUser(int userID);
    void externUpdateUser(QString &hash, int userID);

    void registerNewUser(Persona &persona, QString &hash);
    void receiveError(int error);
    void endProcess();

signals:
    void sendToScreen(const QString &m);
    void endReadFingerprint(QObject *object);
    void sucessRegister(int idUser);
    void responseRegister(const QString &status, const QString &response);
    void setTypeEvent(int event);
    void cancel();
    void finished();

};

#endif // FINGERPRINT_H
