#include "protector.h"
#include <QDebug>

Protector::Protector(const QString &key)
{
    this->key = key;
}

bool Protector::isValid()
{
    return getCalculedKey() == key;
}

QString Protector::getCalculedKey()
{
    QNetworkInterface interface = QNetworkInterface::interfaceFromName("eth0");
    QString mac = interface.hardwareAddress();
    QCryptographicHash hash(QCryptographicHash::Sha1);

    QString k = QString::fromStdString(SALT) + mac;

    hash.addData(k.toUtf8());

    return hash.result().toHex();
}
