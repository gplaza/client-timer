#include "rfid.h"

Rfid::Rfid(QObject *parent) : QObject(parent)
{
    qDebug() << "Init RFID Reader.";
    error = false;

    QObject::connect(this, &Rfid::errorMessage, this, &Rfid::errorReader);

    nfc_init(&context);

    if (context == NULL) {
        qCritical() << "Unable to init libnfc (malloc)";
        error = true;
    }

    if(!error)
        pnd = nfc_open(context, NULL);

    if (pnd == NULL) {
        qCritical() << "Unable to open NFC device.";
        error = true;
        nfc_exit(context);
    }

    thread = new QThread;
}

void Rfid::WaitForTag()
{
    if(!error)
    {
        if(!thread->isRunning())
        {
            //thread = new QThread;
            ReadWorker* readWorker = new ReadWorker(pnd);
            readWorker->moveToThread(thread);

            //Start Thread :
            connect(readWorker, &ReadWorker::error, this, &Rfid::errorReader);
            connect(thread, &QThread::started, readWorker, &ReadWorker::process);
            connect(readWorker, &ReadWorker::resultReady, this, &Rfid::getInfoTag);

            //End Thread :
            connect(this, &Rfid::killThreadReader, readWorker, &ReadWorker::finished);
            connect(readWorker, &ReadWorker::finished, thread, &QThread::quit);
            connect(readWorker, &ReadWorker::finished, readWorker, &ReadWorker::deleteLater);

            thread->start();
        }

    } else {

        emit errorMessage("Error Lector");
    }
}
