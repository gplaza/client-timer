#ifndef FINGERPRINT_H
#define FINGERPRINT_H

#include <persona.h>
#include <buzzer.h>
#include <secugen_sda04.h>
#include <serviceaccess.h>
#include <bdd.h>

class Fingerprint : public SecugenSda04
{
    Q_OBJECT
public:
    Fingerprint(QString serialPort, int AutoOnPin);

    enum resultCompare{
        FINGER_ = 1,
        MARCA_RFID = 2,
        MARCA_INFO = 4
    };

public slots:
    //void checkFingerTouch();
    void verifFingerprint(int userID);
    //void processDataFingerprint();
    // -- function for API --
    void externInsertUser(QString &hash, int typeHash);
    void externDeleteUser(int userID);
    void externUpdateUser(QString &hash, int typeHash, int userID);

    void registerNewUser(Persona &persona, QString &hash);
    void receiveError(int error);

signals:
    void dataReady(QString &id);
    void compareResult(bool result);
    void sucessRegister(int idUser);
    void responseRegister(const QString &status, const QString &response);
    void setTypeEvent(int event);
};

#endif // FINGERPRINT_H
