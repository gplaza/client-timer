#include "serviceaccess.h"
#include "credencial.h"

ServiceAccess::ServiceAccess(SoapClient *soapClient,QObject *objectView, QObject *parent) : QObject(parent)
{
    this->soapClient = soapClient;
    this->objectView = objectView;

    connect(this, &ServiceAccess::onLine,this, &ServiceAccess::on_online);
    connect(this, &ServiceAccess::offLine,this, &ServiceAccess::on_offline);
}

void ServiceAccess::check(QString &id)
{
    QObject* obj = sender();
    const QMetaObject *meta = obj->metaObject();
    QString caller = QString::fromUtf8(meta->className());

    if(caller == "Credencial")
    {
        QSqlRecord record = Bdd::identificationCredencial(id);

        if(!record.isEmpty()) {

            //TODO : nombre,autorizado ??

            persona.setRut(record.value("rut").toString());
            persona.setDv(record.value("dv").toString());
            persona.setFoto(record.value("image").toByteArray());
        }

        persona.setUuid(id);
        persona.setTipoMarca(Persona::MARCA_RFID);
    }

    if(caller == "Fingerprint")
    {
        int ident = id.toInt();
        QSqlRecord fingerprint = Bdd::identificationFingerprint(ident);

        persona.setRut(fingerprint.value("rut").toString());
        persona.setDv(fingerprint.value("dv").toString());
        persona.setUuid(fingerprint.value("hash").toString());
        persona.setTipoMarca(Persona::MARCA_FINGER);

        /*
        Persona *perso = qobject_cast<Persona *>(id);
        persona.setRut(perso->rut());
        persona.setUuid(perso->uuid());
        persona.setDv(perso->dv());
        persona.setTipoMarca(perso->tipoMarca());
        persona.setFingerprintID(perso->fingerprintID());
        persona.setFoto(perso->foto());
        */
    }

    if(persona.tipoMarca() == Persona::MARCA_RFID)
        LOG_INFO("Intento Acceso     : " + persona.rut() + "-" + persona.dv() + " ID tarjeta : " + persona.uuid() + " Tipo marca : " + QString::number(persona.tipoMarca()));
    if(persona.tipoMarca() == Persona::MARCA_FINGER)
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
    soapClient->actionValidarCasino(&persona,acceso);

    if(acceso.idAuth() == -1)
    {
        qCritical() << "Synchronise for retry request (WebService Error)";
        emit synchroniseOffLine(acceso,persona);

    } else {

        qDebug() << "Synchronise BDD with web service data";
        emit synchroniseOnLine(acceso,persona);
    }

    if(persona.foto().isEmpty())
    {
        QByteArray foto = soapClient->getFoto(acceso.rut());
        Bdd::setImage(acceso.rut(),foto);
    }

    finalizeResponse(acceso);
}

void ServiceAccess::on_offline()
{
    qDebug() << "Offline WebService";

    Acceso acceso;
    acceso.setRut(persona.rut());
    acceso.setDv(persona.dv());
    acceso.setUuid(persona.uuid());
    acceso.setDate(QDateTime::currentDateTime());

    if(persona.rut().isEmpty()) {

        acceso.setIdAuth(Acceso::PERSON_NO_EXIST);
        acceso.setName("No existe");

    } else {

        //int autorizado = credencial.value("autorizado").toInt();
        //QString nombre = credencial.value("nombre").toString();

        //qDebug() << "nombre             :" << nombre;
        //qDebug() << "autorizado         :" << autorizado;

        //acceso.setIdAuth(autorizado);
        //acceso.setName(nombre);
    }

    acceso.setTextAuth(Bdd::textAuthentication(acceso));

    emit synchroniseOffLine(acceso,persona);
    finalizeResponse(acceso);
}

void ServiceAccess::finalizeResponse(Acceso &acceso)
{
    LOG_INFO("Resultado Acceso   : " + acceso.toString());
    emit sendToScreen(acceso.textAuth());

    QObject *fotoStudent = objectView->findChild<QObject*>("fotoStudent");
    fotoStudent->setProperty("source", "image://getimagebyrut/" + acceso.rut());

    QObject *lunchInfo = objectView->findChild<QObject*>("lunchInfo");
    lunchInfo->setProperty("text", "Almuerzos disponibles : " + QString::number(acceso.count_lunch()));
    QObject *dinnerInfo = objectView->findChild<QObject*>("dinnerInfo");
    dinnerInfo->setProperty("text", "Cenas disponibles : " + QString::number(acceso.count_dinner()));

    if(acceso.idAuth() == Acceso::PERSON_OK)
    {
        QObject *errorMsg = objectView->findChild<QObject*>("errorMsg");
        errorMsg->setProperty("text", "");

        QString formatedDate = acceso.dateFormated("dd/MM/yy - hh:mm:ss");
        QString name = acceso.name();

        Printer printer("1d90","2060");
        printer.setLine("  U T F S M  ");
        printer.setLine("Fecha   : " + formatedDate);
        printer.setLine("Nombre  : " + name.leftJustified(32, ' ', true));
        printer.setLine("RUT     : " + acceso.rut() + "-" + acceso.dv());
        printer.setLine("Tarjeta : " + acceso.uuid());

        printer.print();

    } else {

        QString msgPrefix = "Mensaje : ";

        QObject *errorMsg = objectView->findChild<QObject*>("errorMsg");
        errorMsg->setProperty("text", msgPrefix + acceso.textAuth());

    }

    QMetaObject::invokeMethod(objectView,"toggle");

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ServiceAccess::finished);
    connect(timer, &QTimer::timeout, this, &ServiceAccess::initUi);
    connect(this, &ServiceAccess::finished, timer, &QTimer::deleteLater);
    timer->start(1000 + 2000);
}

void ServiceAccess::initUi()
{
    QMetaObject::invokeMethod(objectView,"toggle");
}
