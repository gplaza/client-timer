#ifndef TESTBDD_H
#define TESTBDD_H
#include <QtTest/QtTest>
#include <persona.h>
#include <bdd.h>

class TestBdd : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void createPersona();
    void updatePersona();
    void identificationCredencial();
    void cleanupTestCase();
};

#endif // TESTBDD_H
