#ifndef PROTECTOR_H
#define PROTECTOR_H
#include <QString>
#include <QCryptographicHash>
#include <QNetworkInterface>
#define SALT "RASP"

class Protector
{
public:
    Protector(const QString &);
    bool isValid();

private:
    QString getCalculedKey();
    QString key;
};

#endif // PROTECTOR_H
