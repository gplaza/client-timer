#include "credencial.h"
#include <rfid-utils.h>

Credencial::Credencial(QString conn): Rfid(conn) {
    waitForTag();
}

void Credencial::getInfoTag(const ResultRFID &result)
{
    if(result.contains(Rfid::RUT))
    {
        QString rut = result.value(Rfid::RUT);

        if(rut.startsWith("0"))
            rut = rut.right(rut.length() -1);

        qDebug() << "RUT : " << rut;
        emit dataReady(rut);

    } else {

        qWarning() << "This card does not contain RUT";
        readEnable = false;
        QTimer::singleShot(1500, this, SLOT(waitForTag()));
    }
}

QString Credencial::convert_uid(char* string)
{
    qDebug() << "Convert UUID : " << string;

    QString result = "";
    QString tmp = "";
    QString value = QString::fromLocal8Bit(string);

    for(int i=6; i>= 0 ;i = i-2)
    {
        tmp += value.mid(i,2);
    }

    bool ok;
    uint appId = tmp.toUInt(&ok,16);
    result = QString("%1").arg(appId,0,10);

    return result;
}

QString Credencial::hexQString_to_RUT(QString hex)
{
    QString dig = "";
    QString rut = "";
    bool ok;

    rut = (hex.mid(0,1) == "0")? hex.mid(1,8) : hex.mid(0,8);
    dig = hex.mid(9,1);

    if(dig == "B" || dig == "b")
        dig = "K";

    if(dig != verif_rut(rut.toInt()))
    {
        QString hexDig = hex.mid(9,2);
        uint decDig = hexDig.toUInt(&ok,16);
        dig = QString(QChar(decDig));
    }

    return rut + dig;
}

QString Credencial::verif_rut(int rut)
{
    int M=0;
    int S=1;

    for(;rut;rut/=10)
        S=(S+rut%10*(9-M++%6))%11;

    return QString(QChar(S?S+47:75));
}

void Credencial::errorReader(const QString &error)
{
    qCritical() << error;
    Buzzer::instance()->bad();
    emit sendToScreen("error");
    QTimer::singleShot(3000, qApp, SLOT(quit()));
}

