#ifndef SOAPCLIENT_H
#define SOAPCLIENT_H

#include <QUrl>
#include <QDateTime>
#include <QDebug>
#include <time.h>
#include <persona.h>
#include <acceso.h>
#include <soapclient.h>
#include <soapH.h>
#include <configurator.h>
#include <QTcpSocket>

class SoapClient : public QObject
{
    Q_OBJECT
public:
    explicit SoapClient(QObject *parent = 0);
    bool action(Acceso *acceso);
    bool init();
    void action(Persona *persona, Acceso &acceso, QDateTime dateTime = QDateTime::currentDateTime());
    QString action(QString rut, QString dv, QString id, QDateTime dateTime = QDateTime::currentDateTime());

private:
    QString timeout;
    QString endPoint;
    QString soapAction;
    QString usm;
};

#endif // SOAPCLIENT_H
