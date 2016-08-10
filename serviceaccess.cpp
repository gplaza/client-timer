#include "serviceaccess.h"

ServiceAccess::ServiceAccess(Acceso *acceso, QObject *parent) : acceso(acceso), QObject(parent)
{}

void ServiceAccess::check(const QString id)
{
    QObject* obj = sender();
    const QMetaObject *meta = obj->metaObject();
    QString caller = QString::fromUtf8(meta->className());

    acceso->setDate(QDateTime::currentDateTime());
    acceso->setTipoAcceso(Acceso::TYPE_TARJETA);
    acceso->setRut(id);

    if(checkMachineRules() == false)
    {
        qWarning() << "Machine's Rules not passed";
        acceso->setIdAuth(Acceso::OUT_OF_HOURS);

    } else {

        if(caller == "Credencial")
        {
            QSqlRecord record = Bdd::identificationOfflineByRut(id);

            if(!record.isEmpty()) {

                qDebug() << "UserCard finded in local database ...";

                int UserID = record.value("id").toInt();
                int fingerprintUserID = record.value("id_huella").toInt();
                int autorizado = record.value("autorizado").toInt();
                QString name = record.value("nombre").toString();

                acceso->setName(name);
                acceso->setIdAuth(autorizado);

                if(checkUserRules(UserID) == false)
                {
                    qWarning() << "User's Rules not passed";
                    acceso->setIdAuth(Acceso::PERSON_OPTION_ERROR);
                }

                // Test fingerprint only if the user is authorized
                if(fingerprintUserID > 0 && acceso->idAuth() == Acceso::PERSON_OK) {
                    acceso->setTipoAcceso(Acceso::TYPE_HUELLA_SG400);
                    emit verifFingerprint(fingerprintUserID);
                    return;
                }

            } else {

                qWarning() << "UserCard not finded in local database ...";
                acceso->setIdAuth(Acceso::PERSON_NO_EXIST);
                acceso->setName("Unknown user");
            }
        }
    }

    finalizeResponse();
}

bool ServiceAccess::checkUserRules(int UserID)
{
    qDebug() << "Check user's rules ...";

    QMap<QString,QString> options =  Bdd::getUserOption(UserID);
    QDate currentDate = QDate::currentDate();
    QString option;

    qDebug() << "Options user number : " << options.size();

    // Check option 'dateEnd' :
    if(options.contains("dateEnd"))
    {
        option = options["dateEnd"];
        QDate dateEnd = QDate::fromString(option, "dd/MM/yyyy");

        if(currentDate >= dateEnd)
            return false;
    }

    // Check option 'dateStart' :
    if(options.contains("dateStart"))
    {
        option = options["dateStart"];
        QDate dateStart = QDate::fromString(option, "dd/MM/yyyy");

        if(currentDate < dateStart)
            return false;
    }

    return true;
}

bool ServiceAccess::checkMachineRules()
{
    qDebug() << "Check machine's rules ...";

    QDate currentDate = QDate::currentDate();
    QTime currentTime = QTime::currentTime();
    int day = currentDate.dayOfWeek();

    qDebug() << "Current date : "<< currentDate.toString("dddd d MMMM yyyy");

    QSqlQuery horarios = Bdd::getHorarios(day);

    if(horarios.first())
    {
        QString hi = horarios.value("horario_inicio").toString();
        QString hf = horarios.value("horario_fin").toString();
        QTime rhi = QTime::fromString(hi, "hh:mm:ss");
        QTime rhf = QTime::fromString(hf, "hh:mm:ss");

        qDebug() << "Current time : " << currentTime.toString("hh:mm:ss");
        qDebug() << "Current Day  : " << day;
        qDebug() << "Hour valid from " << rhi.toString("hh:mm") << " to " << rhf.toString("hh:mm");

        return (currentTime >= rhi && currentTime <= rhf);

    } else {

        qWarning() << "Error hour : Day not defined";
        return false;
    }
}

void ServiceAccess::checkfingerprint(bool result)
{
    qDebug() << "Check fingerprint ...";

    int autorizado = result? Acceso::PERSON_OK : Acceso::FINGERPRINT_NOT_MATCH;
    acceso->setIdAuth(autorizado);
    finalizeResponse();
}

void ServiceAccess::finalizeResponse()
{
    LOG_INFO("Resultado Local : " + acceso->toString());

    QString rut = acceso->rut();
    int estado = acceso->idAuth();
    int tipoAcceso = acceso->tipoAcceso();
    int tipoCredencial = 0;

    Bdd::registerAccess3(rut, estado, tipoAcceso, tipoCredencial);

    if(acceso->idAuth() == Acceso::PERSON_OK)
    {
        qDebug() << "Access correct";
        Bdd::updatePersonaByAcceso(acceso);
        Buzzer::instance()->good();
        emit openDoor();

    } else {

        qDebug() << "Access not correct";
        Buzzer::instance()->bad();
    }

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ServiceAccess::finished);
    connect(this, &ServiceAccess::finished, timer, &QTimer::deleteLater);
    timer->start(1500);
}
