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
#include <web-service/acceso/accesoH.h>
#include <envH.h>
#include <qprocess.h>

class SoapClient : public QObject
{
    Q_OBJECT
public:
    explicit SoapClient(QObject *parent = 0);
    bool init();
    bool syncro(Persona *persona, Acceso *acceso, QDateTime dateTime);

    // Web service action :
    void actionValidarCasino(Persona *persona, Acceso *acceso, QDateTime dateTime = QDateTime::currentDateTime(), bool sync = false);
    void actionInfoAcceso(Persona *persona,Acceso &acceso, QDateTime dateTime = QDateTime::currentDateTime());
    QByteArray actionGetFoto(QString rut);
    void error(struct soap *soap, QString WebServiceName = "", QMap<QString,QString> inputValues = QMap<QString,QString>(), QString response = "");

private:
    QString timeout;
    QString endPointCasino;
    QString soapActionCasinoValidar;
    QString soapActionCasinoTransaction;
    QString endPointFoto;
    QString soapActionFoto;
    QString endPointAcceso;
    QString soapActionAcceso;
    QString usm;
};

#endif // SOAPCLIENT_H
