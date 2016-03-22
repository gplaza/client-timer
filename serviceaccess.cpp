#include "serviceaccess.h"

ServiceAccess::ServiceAccess(Acceso *acceso, QObject *parent) : acceso(acceso), QObject(parent)
{}

void ServiceAccess::check(const QString id)
{
    QObject* obj = sender();
    const QMetaObject *meta = obj->metaObject();
    QString caller = QString::fromUtf8(meta->className());

    acceso->setDate(QDateTime::currentDateTime());
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

                int fingerprintUserID = record.value("id_huella").toInt();
                int autorizado = record.value("autorizado").toInt();
                QString name = record.value("nombre").toString();

                acceso->setName(name);
                acceso->setIdAuth(autorizado);

                if(fingerprintUserID > 0) {
                    emit verifFingerprint(fingerprintUserID);
                    return;
                }

            } else {

                qWarning() << "UserCard not finded in local database ...";
                acceso->setIdAuth(Acceso::PERSON_NO_EXIST);
            }
        }
    }

    finalizeResponse();
}

bool ServiceAccess::checkMachineRules()
{
    qDebug() << "Check machine's period ...";

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

void ServiceAccess::finalizeResponse()
{
    LOG_INFO("Resultado Local : " + acceso->toString());

    if(acceso->idAuth() == Acceso::PERSON_OK)
    {
        qDebug() << "Save Access";
        Bdd::updatePersonaByAcceso(acceso);
        Bdd::registerAccess();
        Buzzer::instance()->good();
        emit openDoor();

    } else {

        Buzzer::instance()->bad();
    }

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ServiceAccess::finished);
    connect(this, &ServiceAccess::finished, timer, &QTimer::deleteLater);
    timer->start(1500);
}
