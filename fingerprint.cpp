#include "fingerprint.h"
#include "soapclient.h"

Fingerprint::Fingerprint(QString serialport) : SecugenSda04(serialport)
{
    wiringPiSetup();
    pinMode(7, INPUT);

    waitForFinger();
    cancelerTimer = new QTimer(this);

    QObject::connect(this, &Fingerprint::cancel, this, &Fingerprint::endProcess);
    QObject::connect(this, &Fingerprint::sendError, this, &Fingerprint::receiveError);
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
        emit fingerDetected();
        timerFinger->stop();
    }
}

void Fingerprint::processDataFingerprint()
{
    QVariant scanResult = scanFinger();
    int id = scanResult.toInt(); // TODO: conversion direct QString ??

    if(id > -1)
    {
        qDebug() << "Fingerprint finded";
        qDebug() << "User ID : " << id;

        QString ident = QString::number(id);
        emit endReadFingerprint(ident);
        endProcess();

    } else {

        emit sendToScreen("Huella desconocida");
        Piezo::instance()->bad();
        emit cancel();
    }
}

void Fingerprint::endProcess()
{
    // Wait for finger + Wait for tag + Home screen

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Fingerprint::finished);
    connect(timer, &QTimer::timeout, this, &Fingerprint::waitForFinger);
    connect(timer, &QTimer::timeout, timer, &QTimer::deleteLater);
    timer->start(1500);
}

void Fingerprint::externInsertUser(QString &hash)
{
    int idUser = getuserIDavailable();

    if(registerUser(hash,idUser) == 0)
        emit responseRegister("success", QString::number(idUser));
    else
        emit responseRegister("error", "Unable to create new user.");
}

void Fingerprint::externUpdateUser(QString &hash, int userID)
{
    if(deleteUser(userID) == 0)
    {
        int newID = getuserIDavailable();
        if(registerUser(hash,newID) == 0)
            emit responseRegister("success", QString::number(newID));
        else
            emit responseRegister("error", "Unable to create new user.");
    }
}

void Fingerprint::externDeleteUser(int userID)
{
    if(deleteUser(userID) == 0)
        emit responseRegister("success", QString::number(userID));
}

void Fingerprint::registerNewUser(Persona &persona, QString &hash)
{
    int newID = getuserIDavailable();

    if(persona.fingerprintID() != 0)
        deleteUser(persona.fingerprintID());

    if(registerUser(hash,newID) == 0)
    {
        persona.setFingerprintID(newID);
        persona.setHash(hash);
        Bdd::updatePersona(persona);
    }
}
