#include "serviceaccess.h"

ServiceAccess::ServiceAccess(Acceso *acceso, QObject *parent) : acceso(acceso), QObject(parent)
{
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
        persona.setRut("");
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

    emit offLine();
}

void ServiceAccess::on_offline()
{
    /*
    // Check valid credencial
    if(!credenci.isEmpty()) {

        accepted = true;
        credencial.setId(credenci.value("credencial_id").toInt());

        int sentido = credencial.sentido();
        int periodo = credenci.value("periodo_id").toInt();
        bool antiPassBack = credenci.value("antipassback").toBool();

        QDate currentFecha = QDate::currentDate();
        QTime currentHora = QTime::currentTime();
        int dia = currentFecha.dayOfWeek();

        QString activacion = credenci.value("activacion").toString();
        QString expiracion = credenci.value("expiracion").toString();
        activacion = activacion.isEmpty()? currentFecha.toString("yyyy-MM-dd") : activacion;
        expiracion = expiracion.isEmpty()? currentFecha.toString("yyyy-MM-dd") : expiracion;
        QDate activacionDate = QDate::fromString(activacion, "yyyy-MM-dd");
        QDate expiracionDate = QDate::fromString(expiracion, "yyyy-MM-dd");

        qDebug() << "Id cred.     : " << credencial.id();
        qDebug() << "activacion      : " << activacionDate;
        qDebug() << "expiracion      : " << expiracionDate;
        qDebug() << "Antipassback : " << antiPassBack;

        // Check activacion/expiracion
        accepted = (currentFecha >= activacionDate && currentFecha <= expiracionDate);
        qDebug() << "--- Result activacion/expiracion : " << accepted;

        // Check calendario TODO


        // Check valid periodo
        if(periodo != 0 && accepted)
        {
            qDebug() << "Current date : " << currentFecha;
            qDebug() << "Current time : " << currentHora;

            QSqlQuery horarios = Bdd::getHorarios(periodo,dia);

            if(horarios.first())
            {
                accepted = false;
                int cma = 0;

                while (horarios.next()) {

                    QString hi = horarios.value("horario_inicio").toString();
                    QString hf = horarios.value("horario_fin").toString();
                    QTime rhi = QTime::fromString(hi, "hh:mm");
                    QTime rhf = QTime::fromString(hf, "hh:mm");

                    qDebug() << "Valido desde " << hi << " hasta " << hf;

                    if (currentHora >= rhi && currentHora <= rhf)
                    {
                        qDebug() << "Hora : ok";
                        cma = horarios.value("cantidad_maxima_activacion").toInt();
                        accepted = true;

                        // Check Cantidad Max
                        if(cma > 0)
                        {
                            int totalMarc = Bdd::countMarcacionByDate(currentFecha,rhi,rhf,&credencial);
                            accepted = totalMarc <= cma;
                        }

                        break;
                    }
                }
            }
        }

        //Check antiPassBack
        if(antiPassBack && accepted)
        {
            QSqlRecord LastMarcacion = Bdd::getLastMarcacion(&credencial);

            if(!LastMarcacion.isEmpty())
            {
                int lastSentido = LastMarcacion.value("tipo_marcacion").toInt();
                accepted = (lastSentido != sentido);

                qDebug() << "--- Result antiPassBack : " << accepted;
            }
        }

        //Save Marcacion
        save();
    }

    */

    QString casinoName = Configurator::instance()->getConfig("casinoName");

    acceso->setRut(persona.rut());
    acceso->setUuid(persona.uuid());
    acceso->setDate(QDateTime::currentDateTime());
    acceso->setCasinoName(casinoName);

    if(persona.rut().isEmpty()) {

        acceso->setIdAuth(Acceso::PERSON_NO_EXIST);
        acceso->setName("No existe");
        acceso->setCount_casino(0);
        acceso->setRut("000000000");

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
       Buzzer::instance()->good();
       // TODO open door ...

    } else {

        Buzzer::instance()->bad();
    }

    emit finished();
}
