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

    endPointFoto = Configurator::instance()->getConfig("endPointFoto");
    soapActionFoto = Configurator::instance()->getConfig("soapActionFoto");

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
    soap_set_namespaces(soap,foto_namespaces);

    soap->send_timeout = timeout.toInt();
    soap->recv_timeout = timeout.toInt();

    foto::foto1__obtenerFoto* obtenerFoto = new foto::foto1__obtenerFoto();
    foto::foto1__obtenerFotoResponse obtenerFotoResponse;

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
    QString rut = persona->rut().length() > 0 ? persona->complete_rut().rightJustified(10,'0') : "";
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
            // 0;REGISTRO CORRECTO;85;6;0;0193571204;JOSE IGNACIO SEBASTIAN ABARZUA ROJAS;Normal;COMEDORUSM
            // 13;Credencial Inhabilitada;0;0;0;0116222132;
            // 7;Persona no existe;0;0;0;;;Sin impresión;credencial no existe
            // 24;NO EXISTE;58;No;No;04444444404;No No No;NO EXISTE;COMEDORUSM

            if(tipoMarca == Persona::MARCA_RFID) // Web service response rfid card
            {
                int idAuth = resultField.at(0).toInt(&ok);
                QString textAuth = resultField.at(1);

                int count_casino = resultField.at(2).toInt(&ok);
                int count_lunch = resultField.at(3).toInt(&ok);
                int count_dinner = resultField.at(4).toInt(&ok);

                QString rut = resultField.at(5).left(resultField.at(5).length()-1);
                QString dv = resultField.at(5).right(1);

                QString name = "";
                QString info_print = "";
                QString name_cafeteria = "";

                if(resultField.size() >= 6)
                    name = resultField.at(6);
                if(resultField.size() >= 7)
                    info_print= resultField.at(7);
                if(resultField.size() >= 8)
                    name_cafeteria= resultField.at(8);

                while(rut.startsWith("0"))
                    rut = rut.right(rut.length() -1);

                acceso.setIdAuth(idAuth);
                acceso.setTextAuth(textAuth);

                acceso.setRut(rut);
                acceso.setDv(dv);
                acceso.setName(name);
                acceso.setUuid(uuid);

                acceso.setCount_casino(count_casino);
                acceso.setCount_lunch(count_lunch);
                acceso.setCount_dinner(count_dinner);
            }


        } else {

            qCritical() << "Web service error : Invalid response";
        }

        qDebug() << "Web Service raw response : " << result;

    } else {

        qDebug() << "endPoint" << soap->endpoint;

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
