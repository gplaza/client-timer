#include "synchroniser.h"

Synchroniser::Synchroniser(QObject *parent) : QObject(parent)
{
}

void Synchroniser::onLine(Acceso *acceso, Persona &persona)
{
    qDebug() << "Online Synchroniser";

    Bdd::registerAccess();
    bool dataSync = Bdd::checkOfflineData();

    if(dataSync)
    {
        qDebug() << "Fire syncro thread";

        /*
        QThread* thread = new QThread;
         SynchroWorker* synchroWorker = new SynchroWorker(soapClient);
        synchroWorker->moveToThread(thread);

        connect(thread, &QThread::started , synchroWorker, &SynchroWorker::process);
        connect(synchroWorker, &SynchroWorker::finished, thread, &QThread::quit);
        connect(synchroWorker,  &SynchroWorker::finished, synchroWorker, &SynchroWorker::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        thread->start();
        */
    }

    bool personExist = (persona.tipoMarca() == Persona::MARCA_FINGER)? true : Bdd::checkPersona(acceso);
    bool stateCreate = (acceso->idAuth() == Acceso::PERSON_OK || acceso->idAuth() == Acceso::OUT_OF_HOURS || acceso->idAuth() == Acceso::PERSON_NO_LUNCH || acceso->idAuth() == Acceso::PERSON_SERVICE_USED) && !personExist;

    qDebug() << "User exist in local database : " << personExist;
    qDebug() << "Create user in local database : " << stateCreate;

    if(personExist) {
        Bdd::updatePersonaByAcceso(acceso);
        checkFingerPrint(persona);
    }

    if(stateCreate) {
        //TODO : create with a valid counter of lunch and dinner.
        Bdd::createPersona(acceso);

        persona.setFingerprintID(0);
        persona.setRut(acceso->rut());
        checkFingerPrint(persona);
    }

    Bdd::updateCasinoService(acceso);
}

void Synchroniser::offLine(Acceso *acceso, Persona &persona)
{
    qDebug() << "Save offline access";
    Bdd::saveAccess(acceso, persona);
}

void Synchroniser::checkFingerPrint(Persona &persona)
{
    if(persona.fingerprintID() == 0)
    {
        qDebug() << "Search for valid hash";

        // Acceso resultFingerPrint;
        //soapClient->actionInfoAcceso(&persona,resultFingerPrint);
        QString hash = ""; //resultFingerPrint.hash();

        qDebug() << "Hash found : " << hash;

        if(!hash.isEmpty() && hash != "No" && hash != "default")
            emit registerFingerPrint(persona,hash);
    }
}
