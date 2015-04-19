#include "synchroniser.h"

Synchroniser::Synchroniser(SoapClient *soapClient, QObject *parent) : QObject(parent)
{
    this->soapClient = soapClient;
}

void Synchroniser::onLine(Acceso &acceso, Persona &persona)
{
    qDebug() << "Online Synchroniser";

    Bdd::registerAccess();
    bool dataSync = Bdd::checkOfflineData();

    if(dataSync)
    {
        qDebug() << "Fire syncro thread";

        QThread* thread = new QThread;
        SynchroWorker* synchroWorker = new SynchroWorker(soapClient);
        synchroWorker->moveToThread(thread);

        connect(thread, &QThread::started , synchroWorker, &SynchroWorker::process);
        connect(synchroWorker, &SynchroWorker::finished, thread, &QThread::quit);
        connect(synchroWorker,  &SynchroWorker::finished, synchroWorker, &SynchroWorker::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        thread->start();
    }

    bool personExist = (persona.tipoMarca() == Persona::MARCA_FINGER)? true : Bdd::checkPersona(acceso);
    bool stateCreate = (acceso.idAuth() == Acceso::PERSON_OK || acceso.idAuth() == Acceso::PERSON_NO_LUNCH || acceso.idAuth() == Acceso::PERSON_SERVICE_USED);

    if(personExist) {
        Bdd::updatePersonaByAcceso(acceso);
        checkFingerPrint(persona);
    }

    if(!personExist && stateCreate)
        Bdd::createPersona(acceso);
}

void Synchroniser::offLine(Acceso &acceso, Persona &persona)
{
    qDebug() << "Save offline access";
    Bdd::saveAccess(acceso, persona);
    Bdd::registerAccess(Bdd::WEBSERVICE_ERROR);
}

void Synchroniser::checkFingerPrint(Persona &persona)
{
    if(persona.fingerprintID() == 0)
    {
        qDebug() << "Check for fingerPrint";

        Acceso resultFingerPrint;
        persona.setTipoMarca(Persona::MARCA_INFO);

        soapClient->actionInfoAcceso(&persona,resultFingerPrint);
        QString hash = resultFingerPrint.hash();

        if(!hash.isEmpty() && hash != "No" && hash != "default")
            emit registerFingerPrint(persona,hash);
    }
}
