#include <soapclient.h>
#include "web-service/casino/casino.nsmap"
#include "web-service/foto/foto.nsmap"
#include "soap.nsmap"

SoapClient::SoapClient(QObject *parent) : QObject(parent)
{

}

bool SoapClient::init()
{
    timeout = Configurator::instance()->getConfig("timeout");
    endPointCasino = Configurator::instance()->getConfig("endPointCasino");
    soapActionCasinoValidar = Configurator::instance()->getConfig("soapActionCasinoValidar");
    soapActionCasinoTransaction = Configurator::instance()->getConfig("soapActionCasinoTransaction");

    usm = Configurator::instance()->getConfig("usm");

    QUrl url = QUrl(endPointCasino);
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

QByteArray SoapClient::getFoto(QString rut)
{
    QByteArray fotoResult;

    struct soap *soap = soap_new();

    soap->send_timeout = timeout.toInt();
    soap->recv_timeout = timeout.toInt();

    foto::foto1__obtenerFoto* obtenerFoto = new foto::foto1__obtenerFoto();
    foto::foto1__obtenerFotoResponse obtenerFotoResponse;

    std::string temp_rut = rut.toStdString();
    obtenerFoto->rut = &temp_rut;

    qDebug() << "SOAP Web Service foto parameters :";
    qDebug() << "rut     : " << rut;

    if (foto::soap_call___foto1__obtenerFoto(soap,NULL,NULL,obtenerFoto,obtenerFotoResponse) == SOAP_OK)
    {
        foto::xsd__base64Binary *imageResult = obtenerFotoResponse.return_;

        if(imageResult)
        {
            int size = imageResult->__size;
            unsigned char* data = imageResult->__ptr;

            fotoResult = QByteArray((char*) data,size);
        }

    } else {

        this->error(soap);
    }

    soap_destroy(soap); // dealloc serialization data
    soap_end(soap);     // dealloc temp data
    soap_free(soap);    // dealloc 'soap' engine context

    return fotoResult;
}

void SoapClient::actionValidarCasino(Persona *persona,Acceso &acceso, QDateTime dateTime)
{
    struct soap *soap = soap_new1(SOAP_C_UTFSTRING);

    soap->send_timeout = timeout.toInt();
    soap->recv_timeout = timeout.toInt();

    casino::casino1__validar_USCOREcasino* validarCasino = new casino::casino1__validar_USCOREcasino();
    casino::casino1__validar_USCOREcasinoResponse validarCasinoResponse;

    QString result = "";
    QString rut = persona->complete_rut().rightJustified(10,'0');
    QString uuid = persona->uuid();
    int tipoMarca = persona->tipoMarca();

    const char* c_endPoint = endPointCasino.toLocal8Bit().constData();
    const char* c_soapAction = soapActionCasinoValidar.toLocal8Bit().constData();

    std::string temp_rut = rut.toStdString();
    std::string temp_usm = usm.toStdString();
    std::string temp_uuid = uuid.toStdString();
    time_t time = dateTime.toTime_t();

    validarCasino->rut = &temp_rut;
    validarCasino->tipomarca = tipoMarca;
    validarCasino->dispo = &temp_usm;
    validarCasino->dato_USCORErecibido = &temp_uuid;
    validarCasino->fecha = &time;

    qDebug() << "SOAP Web Service parameters :";
    qDebug() << "endPoint   : " << endPointCasino;
    qDebug() << "soapAction : " << soapActionCasinoValidar;
    qDebug() << "tipoMarca  : " << tipoMarca;
    qDebug() << "uuid       : " << QString::fromStdString(temp_uuid);
    qDebug() << "idMaquina  : " << QString::fromStdString(temp_usm);
    qDebug() << "rut        : " << QString::fromStdString(temp_rut);

    if (casino::soap_call___casino1__validar_USCOREcasino(soap,c_endPoint,c_soapAction,validarCasino,validarCasinoResponse) == SOAP_OK)
    {
        std::string* temp_result = validarCasinoResponse.return_;
        result = QString::fromStdString(*temp_result);
        QStringList resultField = result.split(";");
        bool ok;

        // Response sample :
        // 23;YA USO EL SERVICIO DE ALIMENTACION.....;86;4;0;0184480379;CAROLINA ANDREA ZAMORA CASTRO;YA USO EL SERVICIO DE ALIMENTACION.....;COMEDORUSM
        // 0;REGISTRO CORRECTO;85;6;0;0193571204;JOSE IGNACIO SEBASTIAN ABARZUA ROJAS;Normal;COMEDORUSM

        int idAuth = resultField.at(0).toInt(&ok);
        QString textAuth = resultField.at(1);

        int count_casino = resultField.at(2).toInt(&ok);
        int count_lunch = resultField.at(3).toInt(&ok);
        int count_dinner = resultField.at(4).toInt(&ok);

        QString rut = resultField.at(5).left(resultField.at(2).length()-1);
        QString dv = resultField.at(5).right(1);
        QString name = resultField.at(6);
        QString info_print = resultField.at(7);
        QString name_cafeteria = resultField.at(8);

        while(rut.startsWith("0"))
            rut = rut.right(rut.length() -1);


        qDebug() << "Web Service raw response : " << result;

    } else {

        qCritical() << "Error Web service.";
        this->error(soap);
    }

    soap_destroy(soap); // dealloc serialization data
    soap_end(soap);     // dealloc temp data
    soap_free(soap);    // dealloc 'soap' engine context

}

void SoapClient::error(struct soap *soap)
{
    bool state = soap_check_state(soap);

    if (state)
    {
        qCritical() << "Error: soap struct state not initialized";

    } else if (soap->error) {

        const char **fault;
        const char *subCodeFault = NULL;
        const char *reasonFault;
        const char *detailFault;

        fault = soap_faultcode(soap);

        if (!*fault)
            soap_set_fault(soap);

        if (soap->version == 2)
            subCodeFault = soap_check_faultsubcode(soap);

        reasonFault = *soap_faultstring(soap);
        detailFault = soap_check_faultdetail(soap);

        qCritical() << (soap->version ? "SOAP 1." : "Error ") << (soap->version ? (int)soap->version : soap->error);
        qCritical() << "Fault   : " << QString::fromStdString(*fault);
        qCritical() << "subcode : [" << (subCodeFault ? QString::fromStdString(subCodeFault) : "no subcode") << "]";
        qCritical() << "reason  : [" << (reasonFault ? QString::fromStdString(reasonFault) : "no reason") << "]";
        qCritical() << "detail  : [" << (detailFault ? QString::fromStdString(detailFault) : "no detail") << "]";
    }
}

void SoapClient::action(Persona *persona,Acceso &acceso, QDateTime dateTime)
{

    struct soap soap;
    soap_init1(&soap, SOAP_C_UTFSTRING);
    /*
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
    */

}

bool SoapClient::action(Acceso *acceso)
{
    Acceso response;
    Persona persona;

    persona.setRut(acceso->rut());
    persona.setDv(acceso->dv());
    persona.setUuid(acceso->uuid());
    // persona.setTipoMarca(acceso->event().toInt());

    this->action(&persona,response,acceso->date());

    qDebug() << "Synchronisation response : " << response.textAuth() << " El " << response.date().toString("dd/MM/yy - hh:mm:ss") << " Rut:" << response.rut();
    return response.idAuth() != -1;
}
