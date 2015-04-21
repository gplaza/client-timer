#include <soapclient.h>
#include "web-service/casino/casino.nsmap"
#include "web-service/foto/foto.nsmap"
#include "web-service/acceso/acceso.nsmap"
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

    endPointFoto = Configurator::instance()->getConfig("endPointFoto");
    soapActionFoto = Configurator::instance()->getConfig("soapActionFoto");

    endPointAcceso = Configurator::instance()->getConfig("endPointAcceso");
    soapActionAcceso = Configurator::instance()->getConfig("soapActionAcceso");

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

QByteArray SoapClient::actionGetFoto(QString rut)
{
    QByteArray fotoResult;

    struct soap *soap = soap_new();
    soap_set_namespaces(soap,foto_namespaces);

    soap->send_timeout = timeout.toInt();
    soap->recv_timeout = timeout.toInt();

    foto::foto1__obtenerFoto* obtenerFoto = new foto::foto1__obtenerFoto();
    foto::foto1__obtenerFotoResponse obtenerFotoResponse;

    rut = rut.left(rut.length() - 1);
    std::string temp_rut = rut.toStdString();
    obtenerFoto->rut = &temp_rut;

    QByteArray b_endPoint = endPointFoto.toLocal8Bit();
    const char* c_endPoint = b_endPoint.constData();

    QByteArray b_soapAction = soapActionFoto.toLocal8Bit();
    const char* c_soapAction = b_soapAction.constData();

    qDebug() << "SOAP Web Service foto parameters :";
    qDebug() << "endPoint   : " << c_endPoint;
    qDebug() << "soapAction : " << c_soapAction;
    qDebug() << "rut        : " << rut;

    if (foto::soap_call___foto1__obtenerFoto(soap,c_endPoint,c_soapAction,obtenerFoto,obtenerFotoResponse) == SOAP_OK)
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
    soap_set_namespaces(soap,casino_namespaces);

    soap->send_timeout = timeout.toInt();
    soap->recv_timeout = timeout.toInt();

    casino::ns1__validar_USCOREcasino* validarCasino = new casino::ns1__validar_USCOREcasino();
    casino::ns1__validar_USCOREcasinoResponse validarCasinoResponse;

    QString result = "";
    QString rut = persona->rut().length() > 0 ? persona->rut().rightJustified(10,'0') : "";
    QString uuid = persona->uuid();
    int tipoMarca = persona->tipoMarca();

    QByteArray b_endPoint = endPointCasino.toLocal8Bit();
    const char* c_endPoint = b_endPoint.constData();

    QByteArray b_soapAction = soapActionCasinoValidar.toLocal8Bit();
    const char* c_soapAction = b_soapAction.constData();

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
    qDebug() << "endPoint   : " << c_endPoint;
    qDebug() << "soapAction : " << c_soapAction;
    qDebug() << "tipoMarca  : " << tipoMarca;
    qDebug() << "uuid       : " << QString::fromStdString(temp_uuid);
    qDebug() << "idMaquina  : " << QString::fromStdString(temp_usm);
    qDebug() << "rut        : " << QString::fromStdString(temp_rut);

    if (casino::soap_call___ns1__validar_USCOREcasino(soap,c_endPoint,c_soapAction,validarCasino,validarCasinoResponse) == SOAP_OK)
    {
        std::string* temp_result = validarCasinoResponse.return_;
        result = QString::fromStdString(*temp_result);
        QStringList resultField = result.split(";");
        bool ok;

        if(resultField.size() >= 5)
        {
            // Response sample :
            // 23;YA USO EL SERVICIO DE ALIMENTACION.....;86;4;0;0184480379;CAROLINA ANDREA ZAMORA CASTRO;YA USO EL SERVICIO DE ALIMENTACION.....;COMEDORUSM
            // 0;REGISTRO CORRECTO;41;9;0;0194065760;FELIPE ANTONIO AGUILERA ESPINOZA;ESTUDIANTE;Junaeb;COMEDORUSM
            // 13;Credencial Inhabilitada;0;0;0;0116222132;
            // 7;Persona no existe;0;0;0;;;Sin impresión;credencial no existe
            // 24;NO EXISTE;58;No;No;04444444404;No No No;NO EXISTE;COMEDORUSM

            int idAuth = resultField.at(0).toInt(&ok);
            QString textAuth = resultField.at(1);

            int count_casino = resultField.at(2).toInt(&ok);
            int count_lunch = resultField.at(3).toInt(&ok);
            int count_dinner = resultField.at(4).toInt(&ok);

            QString rut = resultField.at(5);

            QString name = "";
            QString info_print = "";
            QString beca_print = "";
            QString name_cafeteria = "";

            if(resultField.size() > 6)
                name = resultField.at(6);
            if(resultField.size() > 7)
                info_print = resultField.at(7);
            if(resultField.size() > 8)
                beca_print = resultField.at(8);
            if(resultField.size() > 9)
                name_cafeteria= resultField.at(9);

            while(rut.startsWith("0"))
                rut = rut.right(rut.length() -1);

            acceso.setIdAuth(idAuth);
            acceso.setTextAuth(textAuth);

            acceso.setRut(rut);
            acceso.setName(name);
            acceso.setUuid(uuid);

            acceso.setCount_casino(count_casino);
            acceso.setCount_lunch(count_lunch);
            acceso.setCount_dinner(count_dinner);
            acceso.setInfo_print(info_print);
            acceso.setBeca_print(beca_print);

        } else {

            qCritical() << "Web service error : Invalid response";
            this->error(soap);
        }

        qDebug() << "Web Service raw response : " << result;

    } else {

        qCritical() << "Error Web service.";
        this->error(soap);
    }

    soap_destroy(soap); // dealloc serialization data
    soap_end(soap);     // dealloc temp data
    soap_free(soap);    // dealloc 'soap' engine context

}

void SoapClient::actionInfoAcceso(Persona *persona,Acceso &acceso, QDateTime dateTime) {

    struct soap *soap = soap_new1(SOAP_C_UTFSTRING);
    soap_set_namespaces(soap,acceso_namespaces);

    soap->send_timeout = timeout.toInt();
    soap->recv_timeout = timeout.toInt();

    acceso::ns1__Modulo_USCOREAcceso_USCOREPuerta* infoAcceso = new acceso::ns1__Modulo_USCOREAcceso_USCOREPuerta();
    acceso::ns1__Modulo_USCOREAcceso_USCOREPuertaResponse infoAccesoResponse;

    QString result = "";
    QString rut = persona->rut();
    QString uuid = persona->uuid();
    int tipoMarca = persona->tipoMarca();

    QByteArray b_endPoint = endPointAcceso.toLocal8Bit();
    const char* c_endPoint = b_endPoint.constData();

    QByteArray b_soapAction = soapActionAcceso.toLocal8Bit();
    const char* c_soapAction = b_soapAction.constData();

    std::string temp_rut = rut.toStdString();
    std::string temp_usm = usm.toStdString();
    std::string temp_uuid = uuid.toStdString();
    time_t time = dateTime.toTime_t();

    unsigned char* pixels = {0};

    acceso::xsd__base64Binary *pimage = acceso::soap_new_xsd__base64Binary(soap);
    pimage->__ptr = pixels;
    pimage->__size = 0;

    infoAcceso->rut = &temp_rut;
    infoAcceso->tipomarca = tipoMarca;
    infoAcceso->numerodispositivo = &temp_usm;
    infoAcceso->dato_USCORErecibido = &temp_uuid;
    infoAcceso->fecha = &time;
    infoAcceso->foto = pimage;

    qDebug() << "SOAP Web Service parameters :";
    qDebug() << "endPoint   : " << c_endPoint;
    qDebug() << "soapAction : " << c_soapAction;
    qDebug() << "tipoMarca  : " << tipoMarca;
    qDebug() << "uuid       : " << QString::fromStdString(temp_uuid);
    qDebug() << "idMaquina  : " << QString::fromStdString(temp_usm);
    qDebug() << "rut        : " << QString::fromStdString(temp_rut);

    if (acceso::soap_call___ns1__Modulo_USCOREAcceso_USCOREPuerta(soap,c_endPoint,c_soapAction,infoAcceso,infoAccesoResponse) == SOAP_OK)
    {
        std::string* temp_result = infoAccesoResponse.return_;
        result = QString::fromStdString(*temp_result);
        QStringList resultField = result.split(";");

        if(resultField.length() >= 8)
        {
            int idAuth = resultField.at(0).toInt();
            QString rut = resultField.at(2);
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
            acceso.setUuid(uuid);
            acceso.setDate(date);
            acceso.setName(name);
            acceso.setTextAuth(textAuth);
            acceso.setHash(hash);

        } else {

            qCritical() << "Web service error : Invalid response";
            this->error(soap);
        }

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

bool SoapClient::syncro(Persona *persona,Acceso &acceso, QDateTime dateTime)
{
    actionValidarCasino(persona,acceso,dateTime);
    return true;
}
