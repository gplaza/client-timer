#ifndef RFID_H
#define RFID_H

#include <nfc/nfc.h>
#include <nfc/nfc-types.h>
#include <stdint.h>
#include <QDebug>
#include <freefare.h>
#include <QThread>

static const nfc_modulation nfcMode = { NMT_ISO14443A, NBR_106 };

class ReadWorker : public QObject
{
    Q_OBJECT
public:
    ReadWorker(nfc_device *pnd)
    {
        qDebug() << "Constructor ReadWorker";

        this->pnd = pnd;
        int init = nfc_initiator_init(this->pnd);

        if(init < 0)
        {
            nfc_perror(pnd, "nfc_initiator_init");
            emit error("Reader init Error");
        }
    }

    ~ReadWorker()
    {
        qDebug() << "Destructor ReadWorker";
    }

public slots:
    void process()
    {
        qDebug() << "Init read RFID process";

        nfc_target nt;
        MifareTag *tags = NULL;
        int response = nfc_initiator_select_passive_target(pnd, nfcMode, NULL, 0, &nt);

        if (response > 0) {

            qDebug() << "New tags receive";
            tags = freefare_get_tags(pnd);
            emit resultReady(tags);
            emit finished();
        }

        if (response < 0) {

            qDebug() << "Nothing to read";
        }
    }

signals:
    void finished();
    void error(QString err);
    void resultReady(MifareTag *tags);

private:
    nfc_device *pnd;

};


class Rfid : public QObject
{
    Q_OBJECT
public:
    explicit Rfid(QObject *parent = 0);

private:
    bool error;
    nfc_device *pnd;
    nfc_context *context;
    QThread *thread;

signals:
    void endReadRFID(QObject *object);
    void errorMessage(const QString &error);
    void killThreadReader();

public slots:
    void WaitForTag();

protected slots:
    virtual void getInfoTag(MifareTag *tags) = 0;
    virtual void errorReader(const QString &error) { qCritical() << error; }

};

#endif // RFID_H

