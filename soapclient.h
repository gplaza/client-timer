#ifndef SOAPCLIENT_H
#define SOAPCLIENT_H

#include <QUrl>
#include <QDateTime>
#include <QDebug>
#include <time.h>
#include <persona.h>
#include <acceso.h>
#include <configurator.h>
#include <QTcpSocket>
#include <web-service/casino/casinoH.h>
#include <web-service/foto/fotoH.h>
#include <envH.h>

class SoapClient : public QObject
{
    Q_OBJECT
public:
    explicit SoapClient(QObject *parent = 0);
    bool action(Acceso *acceso);
    bool init();
    void action(Persona *persona, Acceso &acceso, QDateTime dateTime = QDateTime::currentDateTime());
    QString action(QString rut, QString dv, QString id, QDateTime dateTime = QDateTime::currentDateTime());

    void actionValidarCasino(Persona *persona, Acceso &acceso, QDateTime dateTime = QDateTime::currentDateTime());

    QByteArray getFoto(QString rut);
    void error(struct soap *soap);

private:
    QString timeout;
    QString endPointCasino;
    QString soapActionCasinoValidar;
    QString soapActionCasinoTransaction;
    QString usm;
};

#endif // SOAPCLIENT_H
