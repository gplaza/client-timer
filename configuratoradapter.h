#ifndef CONFIGURATORADAPTER_H
#define CONFIGURATORADAPTER_H
#include <configurator.h>
#include <QObject>
#include <QUrlQuery>
#include <Logger.h>

class ConfiguratorAdapter: public QObject
{
    Q_OBJECT
public:
    ConfiguratorAdapter(QObject *parent=0);

public slots:
    void receiveServerRequest(QHttpRequest* req, QHttpResponse* resp);
    void addPersona(const QByteArray &jsonValue);
    void updatePersona(const QByteArray &jsonValue);
    void response(const QString &status, const QString &response);

signals:
    void deleteFingerPrint(int userID);
    void registerFingerPrint(QString &hash, int typeHash);
    void updateFingerPrint(QString &hash, int typeHash, int userID);
    void userChanged();

private:
    QHttpResponse *resp;


};

#endif // CONFIGURATORADAPTER_H
