#ifndef TESTFINGERPRINT_H
#define TESTFINGERPRINT_H
#include <QtTest/QtTest>
#include <fingerprint.h>
#include <serviceaccess.h>
#include <screen.h>
#include <acceso.h>
#include <persona.h>
#include <synchroniser.h>

class TestFingerprint: public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void testRegisterNewUser();
    void getHashUser();
    void testDeleteUser();
public slots:
    void stubSynchroniserOnLine(Acceso &acceso, Persona &persona);
    void stubSynchroniserOffLine(Acceso &acceso, Persona &persona);

private:
    int userID;
};

#endif // TESTFINGERPRINT_H
