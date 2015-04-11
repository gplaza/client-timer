#ifndef CREDENCIAL_H
#define CREDENCIAL_H
#include <QTimer>

#include <buzzer.h>
#include <rfid.h>

class Credencial : public Rfid
{
    Q_OBJECT
public:
    Credencial(QString conn);
private:
    QString hexQString_to_RUT(QString hex);
    QString convert_uid(char* string);
    QString verif_rut(int rut);

signals:
    void sendToScreen(const QString &m);

public slots:
    void getInfoTag(MifareTag *tags);
    void errorReader(const QString &error);

};

#endif // CREDENCIAL_H
