#include "synchroniser.h"

Synchroniser::Synchroniser(SoapClient *soapClient, QObject *parent) : QObject(parent)
{
    this->soapClient = soapClient;
}

void Synchroniser::onLine(Acceso &acceso, Persona &persona)
{
    qDebug() << "Online Synchroniser";

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

    bool personExist = (persona.tipoMarca() == Persona::MARCA_FINGER)? true : Bdd::checkPersona(persona);
    bool stateCreate = (acceso.idAuth() == Acceso::PERSON_OK || acceso.idAuth() == Acceso::PERSON_NO_LUNCH);

    /*
    if(personExist && acceso.idAuth() != Acceso::PERSON_OK && acceso.idAuth() != Acceso::PERSON_REGISTER_EVENT)
    {
        qDebug() << "Person exist in local database";
        Bdd::updatePersonaByAcceso(acceso);
    }
    */

    if(!personExist && stateCreate)
        Bdd::createPersona(acceso);
}

void Synchroniser::offLine(Acceso &acceso, Persona &persona)
{
    qDebug() << "Save offline access";
    Bdd::saveAccess(acceso, persona);
}

