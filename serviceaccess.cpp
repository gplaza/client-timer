#include "serviceaccess.h"
#include "credencial.h"

ServiceAccess::ServiceAccess(SoapClient *soapClient,QObject *objectView, QObject *parent) : QObject(parent)
{
    this->soapClient = soapClient;
    this->objectView = objectView;

    connect(this, &ServiceAccess::onLine,this, &ServiceAccess::on_online);
    connect(this, &ServiceAccess::offLine,this, &ServiceAccess::on_offline);

    printer = new Printer("1d90","2060");
}

void ServiceAccess::check(QString &id)
{
    QObject* obj = sender();
    const QMetaObject *meta = obj->metaObject();
    QString caller = QString::fromUtf8(meta->className());

    // Check Paper & state of printer
    bool status = printer->checkStatus();
    QMetaObject::invokeMethod(objectView, "changeStatusPrinter", Q_ARG(QVariant, status));

    if(!status)
    {
        emit sendToScreen("Error impresora");
        Buzzer::instance()->bad();

        qCritical() << "Printer error ...";

        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &ServiceAccess::finished);
        connect(this, &ServiceAccess::finished, timer, &QTimer::deleteLater);
        timer->start(1000);

    } else {

        if(caller == "Credencial")
        {
            QSqlRecord record = Bdd::identificationCredencial(id);

            if(!record.isEmpty()) {

                persona.setRut(record.value("rut").toString());
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
            persona.setUuid(fingerprint.value("hash").toString());

            persona.setTipoMarca(Persona::MARCA_FINGER);

            /*
            persona.setFingerprintID(perso->fingerprintID()); TODO : used ?
            */
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
    acceso.setUuid(persona.uuid());
    acceso.setDate(QDateTime::currentDateTime());

    if(persona.rut().isEmpty()) {

        acceso.setIdAuth(Acceso::PERSON_NO_EXIST);
        acceso.setName("No existe");
        acceso.setCount_lunch(0);
        acceso.setCount_dinner(0);

    } else {

        QSqlRecord identity = Bdd::identificationOffline(persona.rut());

        acceso.setIdAuth(identity.value("autorizado").toInt());
        acceso.setName(identity.value("nombre").toString());
        acceso.setCount_lunch(identity.value("count_lunch").toInt());
        acceso.setCount_dinner(identity.value("count_dinner").toInt());
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

    QObject *rutText = objectView->findChild<QObject*>("rutText");
    QString rutString = !acceso.rut().isEmpty()? "RUT : " + acceso.rutFormated() : "";
    rutText->setProperty("text", rutString);

    QObject *nameText = objectView->findChild<QObject*>("nameText");
    QString nameString = !acceso.name().isEmpty()? "Nombre : " + acceso.name().toLower() : "";
    nameText->setProperty("text", nameString);

    if(!acceso.rut().isEmpty())

        if(acceso.idAuth() == Acceso::PERSON_OK)
        {
            QObject *errorMsg = objectView->findChild<QObject*>("errorMsg");
            errorMsg->setProperty("text", "");

            QString formatedDate = acceso.dateFormated("dd/MM/yy - hh:mm:ss");
            QString name = acceso.name();

            printer->setLine(" UTFSM USM:" + Configurator::instance()->getConfig("usm"));
            printer->setLine("Fecha    : " + formatedDate);
            printer->setLine("Nombre   : " + name.leftJustified(31, ' ', true));
            printer->setLine("RUT/Tip. : " + acceso.rutFormated() + '/' + acceso.info_print());

           // Linea 1: UTFSM [nombre Campus] [Número dispositivo] [Nombre dispositivo]
           // Linea 2: Fecha y hora de la transacción
           // Linea 3: [Nombre y apellidos de la persona.]
           // Linea 4: [Rut  y Tipo usuario.]
           // Linea 5 : [Tipo Beca]

           printer->print();

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
