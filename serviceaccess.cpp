#include "serviceaccess.h"

ServiceAccess::ServiceAccess(Acceso *acceso, QObject *parent) : acceso(acceso), QObject(parent)
{
   // connect(this, &ServiceAccess::offLine,this, &ServiceAccess::on_offline);
}

void ServiceAccess::check(const QString id)
{
    QObject* obj = sender();
    const QMetaObject *meta = obj->metaObject();
    QString caller = QString::fromUtf8(meta->className());

    if(caller == "Credencial")
    {
        persona.setRut(id);
        persona.setTipoMarca(Persona::MARCA_RFID);

        QSqlRecord record = Bdd::identificationOfflineByRut(id);

        if(!record.isEmpty()) {

            qDebug() << "UserCard finded in local database ...";

            int userID = record.value("id_huella").toInt();
            persona.setFingerprintID(userID);
            emit verifFingerprint(userID);

        } else {

            qDebug() << "UserCard not finded in local database ...";
            Buzzer::instance()->bad();
            emit finished();
            return;
        }
    }

    if(persona.tipoMarca() == Persona::MARCA_RFID)
        LOG_INFO("Intento Acceso     : ID tarjeta : " + persona.rut() + " Tipo marca : " + QString::number(persona.tipoMarca()));
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

    acceso->setRut(persona.rut());
    acceso->setUuid(persona.uuid());
    acceso->setDate(QDateTime::currentDateTime());

    if(persona.rut().isEmpty()) {

        acceso->setIdAuth(Acceso::PERSON_NO_EXIST);
        acceso->setName("No existe");
        acceso->setRut("000000000");

    } else {

        QSqlRecord identity = Bdd::identificationOfflineByRut(persona.rut());
        acceso->setIdAuth(identity.value("autorizado").toInt());
        acceso->setName(identity.value("nombre").toString());
    }

    Bdd::updatePersonaByAcceso(acceso);
    finalizeResponse();
}

void ServiceAccess::finalizeResponse()
{
    LOG_INFO("Resultado Local : " + acceso->toString());

    if(acceso->idAuth() == Acceso::PERSON_OK)
    {
        qDebug() << "Save Access";
        Bdd::registerAccess();

        Buzzer::instance()->good();
        emit openDoor();

    } else {

        Buzzer::instance()->bad();
    }

    emit finished();
}
