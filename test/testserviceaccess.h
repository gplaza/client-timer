#ifndef TESTSERVICEACCESS_H
#define TESTSERVICEACCESS_H
#include <QtTest/QtTest>
#include <serviceaccess.h>
#include <persona.h>
#include <synchroniser.h>
#include <bdd.h>

class TestServiceAccess : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void testOnlineAccessCard();
    void testOfflineAccessCard();
    void testSynchroniseAccessCard();
    void cleanupTestCase();

private:
    QString endPoint;
    QString soapAction;
    QString usm;
    int timeout;
};

#endif // TESTSERVICEACCESS_H
