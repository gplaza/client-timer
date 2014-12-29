#include <soapclient.h>
#include "Ws_USCOREPrincipalPortBinding.nsmap"

SoapClient::SoapClient(QObject *parent) : QObject(parent)
{

}

bool SoapClient::init()
{
    timeout = Configurator::instance()->getConfig("timeout");
    endPoint = Configurator::instance()->getConfig("endPoint");
    soapAction = Configurator::instance()->getConfig("soapAction");
    usm = Configurator::instance()->getConfig("usm");

    QUrl url = QUrl(endPoint);
    QTcpSocket *service = new QTcpSocket();
    QString host = url.host();
    service->connectToHost(host,80);

    qDebug() << "Accessibility test on " << host;

    if (!service->waitForConnected(1500)) {
        qCritical() << "OffLine error : " << service->error();
        service->deleteLater();
        return false;
    }

    service->deleteLater();
    return true;
}

void SoapClient::action(Persona *persona,Acceso &acceso, QDateTime dateTime)
{
    struct soap soap;
    soap_init1(&soap, SOAP_C_UTFSTRING);

    soap.send_timeout = timeout.toInt();
    soap.recv_timeout = timeout.toInt();

    ns1__Modulo_USCOREAcceso_USCOREPuerta* controlPuerta = new ns1__Modulo_USCOREAcceso_USCOREPuerta();
    ns1__Modulo_USCOREAcceso_USCOREPuertaResponse* controlPuertaResponse = new SOAP_CMAC ns1__Modulo_USCOREAcceso_USCOREPuertaResponse();

    QString result = "";
    QString rut = persona->complete_rut();
    QString uuid = persona->uuid();
    int tipoMarca = persona->tipoMarca();

    QByteArray b_endPoint = endPoint.toLocal8Bit();
    const char* c_endPoint = b_endPoint.constData();

    QByteArray b_soapAction = soapAction.toLocal8Bit();
    const char* c_soapAction = b_soapAction.constData();

    std::string temp_rut = rut.toStdString();
    std::string* p_rut = &temp_rut;
    std::string temp_usm = usm.toStdString();
    std::string* p_usm = &temp_usm;
    std::string temp_uuid = uuid.toStdString();
    std::string* p_uuid = &temp_uuid;

    time_t time = dateTime.toTime_t();

    int size = persona->foto().size();
    unsigned char* pixels = (unsigned char*)persona->foto().data();

    xsd__base64Binary *pimage = soap_new_xsd__base64Binary(&soap);
    pimage->__ptr = pixels;
    pimage->__size = size;

    controlPuerta->rut = p_rut;
    controlPuerta->tipomarca = tipoMarca;
    controlPuerta->numerodispositivo = p_usm;
    controlPuerta->dato_USCORErecibido = p_uuid;
    controlPuerta->fecha = &time;
    controlPuerta->foto = pimage;

    qDebug() << "SOAP Web Service parameters :";
    qDebug() << "endPoint   : " << endPoint;
    qDebug() << "soapAction : " << soapAction;
    qDebug() << "tipoMarca  : " << tipoMarca;
    qDebug() << "uuid       : " << QString::fromStdString(temp_uuid);
    qDebug() << "idMaquina  : " << QString::fromStdString(temp_usm);
    qDebug() << "rut        : " << QString::fromStdString(temp_rut);
    qDebug() << "foto size  : " << size;

    if (soap_call___ns1__Modulo_USCOREAcceso_USCOREPuerta(&soap,c_endPoint,c_soapAction,controlPuerta,controlPuertaResponse) == SOAP_OK)
    {
        std::string* temp_result = controlPuertaResponse->return_;

        result = QString::fromStdString(*temp_result);
        QStringList resultField = result.split(";");
        bool ok;

        if(tipoMarca == Acceso::MARCA_RFID) // Web service response rfid card
        {
            if(resultField.length() < 6)
            {
                qCritical() << "response format error : " << result;
                acceso.setIdAuth(-1);
                return;
            }

            int idAuth = resultField.at(0).toInt(&ok);
            QString textAuth = resultField.at(1);
            QString rut = resultField.at(2).left(resultField.at(2).length()-1);
            QString dv = resultField.at(2).right(1);
            QString timeZoneAb = resultField.at(3).contains("CLT")? "CLT" : "CLST";
            QDateTime date = dateTime.fromString(resultField.at(3),"ddd MMM dd hh:mm:ss " + timeZoneAb + " yyyy");
            QString name = resultField.at(4);
            QString event = resultField.at(5);

            while(rut.startsWith("0"))
                rut = rut.right(rut.length() -1);

            acceso.setDate(date);
            acceso.setIdAuth(idAuth);
            acceso.setTextAuth(textAuth);
            acceso.setUuid(uuid);
            acceso.setRut(rut);
            acceso.setDv(dv);
            acceso.setName(name);
            acceso.setEvent(event);
        }

        if(tipoMarca == Acceso::MARCA_FINGER) // Web service response fingerprint
        {
            if(resultField.length() < 6)
            {
                qCritical() << "response format error : " << result;
                acceso.setIdAuth(-1);
                return;
            }

            int idAuth = resultField.at(0).toInt();
            QString textAuth = resultField.at(1);
            QString rut = resultField.at(2).left(resultField.at(2).length()-1);
            QString dv = resultField.at(2).right(1);
            QString timeZoneAb = resultField.at(3).contains("CLT")? "CLT" : "CLST";
            QDateTime date = dateTime.fromString(resultField.at(3),"ddd MMM dd hh:mm:ss " + timeZoneAb + " yyyy");
            QString name = resultField.at(4);
            QString event = resultField.at(5);

            while(rut.startsWith("0"))
                rut = rut.right(rut.length() -1);

            acceso.setDate(date);
            acceso.setIdAuth(idAuth);
            acceso.setTextAuth(textAuth);
            acceso.setUuid(uuid);
            acceso.setRut(rut);
            acceso.setDv(dv);
            acceso.setName(name);
            acceso.setEvent(event);

            if(uuid.length() > 10)
                acceso.setUuid("");
        }

        if(tipoMarca == Acceso::MARCA_INFO) // Web service response fingerprint information
        {

            if(resultField.length() < 8)
            {
                qCritical() << "response format error." << result;
                acceso.setIdAuth(-1);
                return;
            }

            int idAuth = resultField.at(0).toInt();
            QString event = resultField.length() > 1 ? resultField.at(1) : "";
            QString rut = resultField.at(2).left(resultField.at(2).length()-1);
            QString dv = resultField.at(2).right(1);
            QString timeZoneAb = resultField.at(3).contains("CLT")? "CLT" : "CLST";
            QDateTime date = dateTime.fromString(resultField.at(3),"ddd MMM dd hh:mm:ss " + timeZoneAb + " yyyy");
            QString name = resultField.length() > 3 ? resultField.at(4) : "";
            QString textAuth = resultField.at(5);
            uuid = resultField.at(6);
            QString hash = resultField.at(7);

            //If DB Santa Maria don't have the UUID replace by UUID local if exist
            if (uuid.isEmpty() && !persona->uuid().isEmpty())
                uuid = persona->uuid();

            while(rut.startsWith("0"))
                rut = rut.right(rut.length() -1);

            acceso.setIdAuth(idAuth);
            acceso.setRut(rut);
            acceso.setDv(dv);
            acceso.setUuid(uuid);
            acceso.setEvent(event);
            acceso.setDate(date);
            acceso.setName(name);
            acceso.setTextAuth(textAuth);
            acceso.setHash(hash);
        }

        qDebug() << "Web Service raw response : " << result;
        acceso.setEventPrint((acceso.idAuth() == Acceso::PERSON_OK || acceso.idAuth() == Acceso::PERSON_REGISTER_EVENT)? Acceso::Printed : Acceso::Noprinted);

    } else {

        qCritical() << "Error Web service.";

        if (soap_check_state(&soap))
        {
            qCritical() << "Error: soap struct state not initialized";

        } else if (soap.error) {

            const char **fault;
            const char *subCodeFault = NULL;
            const char *reasonFault;
            const char *detailFault;

            fault = soap_faultcode(&soap);
            if (!*fault)
                soap_set_fault(&soap);
            if (soap.version == 2)
                subCodeFault = soap_check_faultsubcode(&soap);

            reasonFault = *soap_faultstring(&soap);
            detailFault = soap_check_faultdetail(&soap);

            qCritical() << (soap.version ? "SOAP 1." : "Error ") << (soap.version ? (int)soap.version : soap.error);
            qCritical() << "Fault   : " << QString::fromStdString(*fault);
            qCritical() << "subcode : [" << (subCodeFault ? QString::fromStdString(subCodeFault) : "no subcode") << "]";
            qCritical() << "reason  : [" << (reasonFault ? QString::fromStdString(reasonFault) : "no reason") << "]";
            qCritical() << "detail  : [" << (detailFault ? QString::fromStdString(detailFault) : "no detail") << "]";
        }

        acceso.setIdAuth(-1);
        acceso.setUuid("-");
        acceso.setTextAuth("WebService Error");
        acceso.setDate(dateTime);
        acceso.setRut(persona->rut());
        acceso.setDv(persona->dv());
        acceso.setName("-");
        acceso.setEvent("Error");
        acceso.setHash("");
    }

    soap_destroy(&soap); // remove deserialized class instances (C++ only)
    soap_end(&soap); // clean up and remove deserialized data
    soap_done(&soap); // detach context (last use and no longer in scope)

}

bool SoapClient::action(Acceso *acceso)
{
    Acceso response;
    Persona persona;

    persona.setRut(acceso->rut());
    persona.setDv(acceso->dv());
    persona.setUuid(acceso->uuid());
    persona.setTipoMarca(acceso->event().toInt());

    this->action(&persona,response,acceso->date());

    qDebug() << "Synchronisation response : " << response.textAuth() << " El " << response.date().toString("dd/MM/yy - hh:mm:ss") << " Rut:" << response.rut();
    return response.idAuth() != -1;
}
