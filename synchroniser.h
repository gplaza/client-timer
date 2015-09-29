#ifndef SYNCHRONISER_H
#define SYNCHRONISER_H
#include <QDateTime>
#include <QTextStream>
#include <QDebug>

#include <bdd.h>
#include <acceso.h>

class Synchroniser : public QObject
{
    Q_OBJECT
public:
    Synchroniser(QObject *parent = 0);

signals:
    void registerFingerPrint(Persona &persona, QString &hash);

private:
    void checkFingerPrint(Persona &persona);

public slots:
    void onLine(Acceso *acceso, Persona &persona);
    void offLine(Acceso *acceso, Persona &persona);

};

#endif // SYNCHRONISER_H
