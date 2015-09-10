#include "fingerprint.h"
#include "soapclient.h"

Fingerprint::Fingerprint(QString serialport) : SecugenSda04(serialport)
{
    if(!error)
    {
        wiringPiSetup();
        pinMode(7, INPUT);

        waitForFinger();
        cancelerTimer = new QTimer(this);

        QObject::connect(this, &Fingerprint::sendError, this, &Fingerprint::receiveError);
    }
}

void Fingerprint::receiveError(int error)
{
    if(error == Fingerprint::ERROR_TIMEOUT)
    {
        qCritical() << "Error : Fingerprint reader timeout";
        Bdd::registerAccess(Bdd::TIMEOUT_FINGERPRINT);
    }

    if(error == Fingerprint::ERROR_IDENTIFY_FAILED)
    {
        qCritical() << "Error : Fingerprint unknown";
        Bdd::registerAccess(Bdd::UNKNOWN_FINGERPRINT);
    }

    if(error == Fingerprint::ERROR_USER_NOT_FOUND)
    {
        qCritical() << "Error : User not found";
        emit responseRegister("error","User not found in hardware reader");
    }

    if(error == Fingerprint::ERROR_FLASH_WRITE_ERROR)
    {
        qCritical() << "Error : User not found";
        emit responseRegister("error","Error on write reader flash card");
    }

    if(error == Fingerprint::ERROR_INSUFFICIENT_DATA)
    {
        qCritical() << "Error : Hash invalid";
        emit responseRegister("error","Hash invalid");
    }
}

void Fingerprint::checkFingerTouch()
{
    if(digitalRead(7) == LOW)
    {
        timerFinger->stop();
        emit fingerDetected();
    }
}

void Fingerprint::processDataFingerprint()
{
    QVariant scanResult = scanFinger();
    int id = scanResult.toInt();

    qDebug() << "Fingerprint finded";
    qDebug() << "User ID : " << id;

    if(id > 0)
    {
        QString ident = QString::number(id);
        Buzzer::instance()->good();
        emit dataReady(ident);

    } else {

        emit sendToScreen("Huella          desconocida");
        Buzzer::instance()->bad();
        emit unknownFinger();
    }
}

void Fingerprint::externInsertUser(QString &hash, int typeHash)
{
    int userID = getuserIDavailable();

    if(registerUser(hash,userID,false,typeHash) == 0)
        emit responseRegister("success", QString::number(userID));
    else
        emit responseRegister("error", "Unable to create new user.");
}

void Fingerprint::externUpdateUser(QString &hash, int typeHash, int userID)
{
    if(registerUser(hash,userID,true,typeHash) == 0)
        emit responseRegister("success", QString::number(userID));
    else
        emit responseRegister("error", "Unable to update user.");
}

void Fingerprint::externDeleteUser(int userID)
{
    if(deleteUser(userID) == 0)
        emit responseRegister("success", QString::number(userID));
    else
        emit responseRegister("error", "Unable to delete user.");
}

void Fingerprint::registerNewUser(Persona &persona, QString &hash)
{
    bool userExist = persona.fingerprintID() != 0;
    int userID = userExist? persona.fingerprintID() : getuserIDavailable(); //TODO : verif si ok ...

    if(registerUser(hash,userID,userExist) == 0)
    {
        persona.setFingerprintID(userID);
        persona.setHash(hash);
        Bdd::updatePersona(persona);
    }
}
