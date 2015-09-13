#include "serviceaccess.h"

ServiceAccess::ServiceAccess(SoapClient *soapClient, Acceso *acceso, QObject *parent) : soapClient(soapClient), acceso(acceso), QObject(parent)
{
    connect(this, &ServiceAccess::onLine,this, &ServiceAccess::on_online);
    connect(this, &ServiceAccess::offLine,this, &ServiceAccess::on_offline);
}

void ServiceAccess::check(const QString id)
{
    QObject* obj = sender();
    const QMetaObject *meta = obj->metaObject();
    QString caller = QString::fromUtf8(meta->className());

    if(caller == "Credencial")
    {
        persona.setUuid(id);
        persona.setRut(""); // TODO : change to pointer the object Persona.
        persona.setTipoMarca(Persona::MARCA_RFID);

        QSqlRecord record = Bdd::identificationCredencial(id);

        if(!record.isEmpty()) {

            qDebug() << "User finded in local database ...";
            persona.setRut(record.value("rut").toString());
            persona.setExistFoto(!record.value("foto").toString().isEmpty());
            persona.setFingerprintID(record.value("id_huella").toInt());
        }

    }

    if(caller == "Fingerprint")
    {
        QSqlRecord fingerprint = Bdd::identificationFingerprint(id);

        persona.setRut(fingerprint.value("rut").toString());
        persona.setUuid(fingerprint.value("hash").toString());
        persona.setExistFoto(!fingerprint.value("foto").toString().isEmpty());
        persona.setFingerprintID(id.toInt());

        persona.setTipoMarca(Persona::MARCA_FINGER);
    }

    if(persona.tipoMarca() == Persona::MARCA_RFID)
        LOG_INFO("Intento Acceso     : ID tarjeta : " + persona.uuid() + " Tipo marca : " + QString::number(persona.tipoMarca()));
    if(persona.tipoMarca() == Persona::MARCA_FINGER)
        LOG_INFO("Intento Acceso     : " + persona.rut() + " Hash size : " + QString::number(persona.uuid().size()) + " Tipo marca : " + QString::number(persona.tipoMarca()));

    bool result  = soapClient->init();

    if(result)
        emit onLine();
    else
        emit offLine();

}

void ServiceAccess::on_online()
{
    qDebug() << "Online WebService";
    soapClient->actionValidarCasino(&persona,acceso);

    if(acceso->idAuth() == -1)
    {
        qCritical() << "Synchronise for retry request (WebService Error)";
        emit offLine();
        return;

    } else {

        qDebug() << "Synchronise BDD with web service data";
        emit synchroniseOnLine(acceso,persona);
    }

    if(persona.existFoto() == false && acceso->idAuth() != Acceso::PERSON_NO_EXIST && acceso->idAuth() != Acceso::PERSON_CRED_NO_EXIST) {
        QByteArray foto = soapClient->actionGetFoto(acceso->rut());
        if(!foto.isEmpty())
        {
            Bdd::setImage(acceso->rut(),foto);
            emit fotoChanged();
        }
    }

    finalizeResponse();
}

void ServiceAccess::on_offline()
{
    qDebug() << "Offline WebService";

    QString casinoName = Configurator::instance()->getConfig("casinoName");

    acceso->setRut(persona.rut());
    acceso->setUuid(persona.uuid());
    acceso->setDate(QDateTime::currentDateTime());
    acceso->setCasinoName(casinoName);

    if(persona.rut().isEmpty() && persona.uuid().isEmpty()) {

        acceso->setIdAuth(Acceso::PERSON_NO_EXIST);
        acceso->setName("No existe");
        acceso->setCount_casino(0);

    } else {

        QSqlRecord identity = (!persona.rut().isEmpty())? Bdd::identificationOfflineByRut(persona.rut()) : Bdd::identificationOfflineByUuid(persona.uuid());

        acceso->setIdAuth(identity.value("autorizado").toInt());
        acceso->setName(identity.value("nombre").toString());
        acceso->setCount_lunch(identity.value("count_lunch").toInt());
        acceso->setCount_dinner(identity.value("count_dinner").toInt());
    }

    acceso->setCount_casino(Bdd::casinoService());
    acceso->setTextAuth(Bdd::textAuthentication(acceso));

    emit synchroniseOffLine(acceso,persona);
    finalizeResponse();
}

void ServiceAccess::finalizeResponse()
{
    LOG_INFO("Resultado Local : " + acceso->toString());

    if(acceso->idAuth() == Acceso::PERSON_OK)
    {
        QString formatedDate = QDateTime::currentDateTime().toString("dd/MM/yy - hh:mm:ss");
        QString name = acceso->name();
    }

    emit finished();
}
