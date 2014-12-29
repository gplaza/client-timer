#include "serviceaccess.h"
#include "credencial.h"

ServiceAccess::ServiceAccess(SoapClient *soapClient, QObject *parent) : QObject(parent)
{
    this->soapClient = soapClient;
    connect(this, &ServiceAccess::onLine,this, &ServiceAccess::on_online);
    connect(this, &ServiceAccess::offLine,this, &ServiceAccess::on_offline);
}

void ServiceAccess::check(QObject *object)
{
    Persona *perso = qobject_cast<Persona *>(object);
    persona.setRut(perso->rut());
    persona.setUuid(perso->uuid());
    persona.setDv(perso->dv());
    persona.setTipoMarca(perso->tipoMarca());
    persona.setFingerprintID(perso->fingerprintID());
    persona.setFoto(perso->foto());

    if(event == INFO_FINGERPRINT)
        persona.setTipoMarca(4);
    if(event == RECORD_FINGERPRINT)
        persona.setTipoMarca(1);

    if(event != RECORD_FINGERPRINT)
        LOG_INFO("Intento Acceso     : " + persona.rut() + "-" + persona.dv() + " ID tarjeta : " + persona.uuid() + " Tipo marca : " + QString::number(persona.tipoMarca()));
    else
        LOG_INFO("Intento Acceso     : " + persona.rut() + "-" + persona.dv() + " Hash size : " + QString::number(persona.uuid().size()) + " Tipo marca : " + QString::number(persona.tipoMarca()));

    bool result  = soapClient->init();

    if(result)
        emit onLine();
    else
        emit offLine();
}

void ServiceAccess::on_online()
{
    qDebug() << "Online WebService";

    Acceso acceso;
    soapClient->action(&persona,acceso);

    if(acceso.idAuth() == -1)
    {
        qCritical() << "Synchronise for retry request (WebService Error)";
        emit synchroniseOffLine(acceso,persona);

    } else {

        qDebug() << "Synchronise BDD with web service data";
        emit synchroniseOnLine(acceso,persona);
    }

    if(event == ServiceAccess::INFO_FINGERPRINT)
    {
        qDebug() << "Send response web service to fingerprint Manager";

        emit hashResponse(acceso);

        if(acceso.textAuth() == " Persona valida")
            finalizeResponse(acceso);

    } else {
        finalizeResponse(acceso);
    }
}

void ServiceAccess::on_offline()
{
    qDebug() << "Offline WebService";

    QSqlRecord credencial = Bdd::identificationCredencial(&persona);

    Acceso acceso;
    acceso.setRut(persona.rut());
    acceso.setDv(persona.dv());
    acceso.setUuid(persona.uuid());
    acceso.setDate(QDateTime::currentDateTime());

    if(credencial.isEmpty()) {

        acceso.setIdAuth(Acceso::PERSON_NO_EXIST);
        acceso.setName("No existe");
        acceso.setEventPrint(Acceso::Noprinted);

    } else {

        int autorizado = credencial.value("autorizado").toInt();
        QString nombre = credencial.value("nombre").toString();

        qDebug() << "nombre             :" << nombre;
        qDebug() << "autorizado         :" << autorizado;

        acceso.setIdAuth(autorizado);
        acceso.setName(nombre);
        acceso.setEventPrint(Acceso::Printed);
    }

    acceso.setTextAuth(Bdd::textAuthentication(acceso));

    emit synchroniseOffLine(acceso,persona);
    finalizeResponse(acceso);
}

void ServiceAccess::finalizeResponse(Acceso &acceso)
{
    LOG_INFO("Resultado Acceso   : " + acceso.toString());
    emit sendToScreen(acceso.textAuth());

    if(acceso.eventPrint() == Acceso::Printed)
    {
        QString formatedDate = acceso.dateFormated("dd/MM/yy - hh:mm:ss");
        QString name = acceso.name();
        QString evento = acceso.event().toUpper();

        Printer printer("1d90","2060");
        printer.setLine("  U T F S M  ");
        printer.setLine(" * * " + evento + " * * ");
        printer.setLine("Fecha   : " + formatedDate);
        printer.setLine("Nombre  : " + name.leftJustified(32, ' ', true));
        printer.setLine("RUT     : " + acceso.rut() + "-" + acceso.dv());
        printer.setLine("Tarjeta : " + acceso.uuid());

        printer.print();
    }

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ServiceAccess::finished);
    connect(this, &ServiceAccess::finished, timer, &QTimer::deleteLater);
    timer->start(1500);
}

void ServiceAccess::setTypeEvent(int event)
{
    qDebug() << "Set tipo event to : " << event;
    this->event = event;
}
