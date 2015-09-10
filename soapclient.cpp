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

void SoapClient::actionValidarCasino(Persona *persona,Acceso *acceso, QDateTime dateTime, bool sync)
{
    struct soap *soap = soap_new1(SOAP_C_UTFSTRING);
    soap_set_namespaces(soap,casino_namespaces);

    soap->send_timeout = timeout.toInt();
    soap->recv_timeout = timeout.toInt();

    casino::casino1__Modulo_USCORECasino* validarCasino = new casino::casino1__Modulo_USCORECasino();
    casino::casino1__Modulo_USCORECasinoResponse validarCasinoResponse;

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
    validarCasino->numerodispositivo = &temp_usm;
    validarCasino->dato_USCORErecibido = &temp_uuid;
    validarCasino->fecha = &time;

    QMap<QString,QString> inputValues;
    inputValues.insert("endPoint",c_endPoint);
    inputValues.insert("soapAction",c_soapAction);
    inputValues.insert("tipoMarca",QString::number(tipoMarca));
    inputValues.insert("fecha",dateTime.toString("ddd MMM dd hh:mm:ss yyyy"));
    inputValues.insert("uuid",QString::fromStdString(temp_uuid));
    inputValues.insert("idMaquina",QString::fromStdString(temp_usm));
    inputValues.insert("rut",QString::fromStdString(temp_rut));

    if(!sync)
    {
        qDebug() << "SOAP Web Service parameters :";
        qDebug() << "endPoint   : " << c_endPoint;
        qDebug() << "soapAction : " << c_soapAction;
        qDebug() << "tipoMarca  : " << tipoMarca;
        qDebug() << "fecha      : " << dateTime.toString("ddd MMM dd hh:mm:ss yyyy");
        qDebug() << "uuid       : " << QString::fromStdString(temp_uuid);
        qDebug() << "idMaquina  : " << QString::fromStdString(temp_usm);
        qDebug() << "rut        : " << QString::fromStdString(temp_rut);

    } else {

        qDebug() << "Syncro fecha      : " << dateTime.toString("ddd MMM dd hh:mm:ss yyyy");
    }

    if (casino::soap_call___casino1__Modulo_USCORECasino(soap,c_endPoint,c_soapAction,validarCasino,validarCasinoResponse) == SOAP_OK)
    {
        std::string* temp_result = validarCasinoResponse.return_;
        result = QString::fromStdString(*temp_result);
        QStringList resultField = result.split(";");

        if(resultField.size() >= 5)
        {
            // Response sample :
            // 0;REGISTRO CORRECTO;206;19;0;0150691222;ALEJANDRO CAMILO OYARZUN PUENTES;FUNCIONARIO;Normal;COMEDORUSM
            // 18;Fuera de horario;0;0;0;0184571900;GIOVANNI EDUARDO ARAVENA MORALES;Sin impresión;CASINOVAL3

            bool convertIdAuth;
            bool ok;

            int idAuth = resultField.at(0).toInt(&convertIdAuth);
            QString textAuth = resultField.at(1);

            int count_casino = resultField.at(2).toInt(&ok);
            int count_lunch = resultField.at(3).toInt(&ok);
            int count_dinner = resultField.at(4).toInt(&ok);

            QString rut = resultField.at(5);

            QString name = "";
            QString info_print = "";
            QString beca_print = "";
            QString casinoName = Configurator::instance()->getConfig("casinoName");

            if(resultField.size() > 6)
                name = resultField.at(6);
            if(resultField.size() > 7)
                info_print = resultField.at(7);
            if(resultField.size() > 8)
                beca_print = resultField.at(8);
            if(resultField.size() > 9) {
                if (casinoName != resultField.at(9)) {
                    casinoName = resultField.at(9).simplified();
                    Configurator::instance()->setConfigByKey("casinoName", casinoName);
                }
            }

            while(rut.startsWith("0"))
                rut = rut.right(rut.length() -1);

            acceso->setIdAuth(idAuth);
            acceso->setTextAuth(textAuth);

            acceso->setRut(rut);
            acceso->setName(name);
            acceso->setUuid(uuid);

            acceso->setCount_casino(count_casino);
            acceso->setCount_lunch(count_lunch);
            acceso->setCount_dinner(count_dinner);
            acceso->setInfo_print(info_print);
            acceso->setBeca_print(beca_print);
            acceso->setCasinoName(casinoName);

            if(!convertIdAuth) {

                acceso->setIdAuth(100);
                acceso->setTextAuth("Error, comuníquese con el encargado para normalizar su situación.");

                qCritical() << "Error conversion parameter idAuth to integer.";
                this->error(soap, "validar_casino", inputValues, result);
            }

        } else {

            qCritical() << "Number of field incorrect.";
            this->error(soap, "validar_casino", inputValues, result);
        }

        qDebug() << (sync? "Syncro " : "") << "Web Service raw response : " << result;

    } else {

        qCritical() << "Error Web service.";
        this->error(soap, "validar_casino", inputValues);
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
    qDebug() << "endPoint             : " << c_endPoint;
    qDebug() << "soapAction           : " << c_soapAction;
    qDebug() << "tipoMarca            : " << tipoMarca;
    qDebug() << "uuid (dato_recibido) : " << QString::fromStdString(temp_uuid);
    qDebug() << "idMaquina            : " << QString::fromStdString(temp_usm);
    qDebug() << "rut                  : " << QString::fromStdString(temp_rut);

    if (acceso::soap_call___ns1__Modulo_USCOREAcceso_USCOREPuerta(soap,c_endPoint,c_soapAction,infoAcceso,infoAccesoResponse) == SOAP_OK)
    {
        std::string* temp_result = infoAccesoResponse.return_;
        result = QString::fromStdString(*temp_result);
        QStringList resultField = result.split(";");

        if(resultField.length() >= 6)
        {
            QString hashASI = "";
            QString hashSG400 = "";
            QString uuid = "";

            int idAuth = resultField.at(0).toInt();
            QString rut = resultField.at(2);
            QString cleanDate = resultField.at(3);
            cleanDate.replace("GMT-03:00", "");
            QDateTime date = dateTime.fromString(cleanDate,"ddd MMM dd hh:mm:ss  yyyy");
            QString name = resultField.length() > 3 ? resultField.at(4) : "";
            QString textAuth = resultField.at(5);
            if(resultField.length() > 6)
                uuid = resultField.at(6);
            if(resultField.length() > 7)
                hashASI = resultField.at(7);
            if(resultField.length() > 8)
                hashSG400 = resultField.at(8);

            while(rut.startsWith("0"))
                rut = rut.right(rut.length() -1);

            acceso.setIdAuth(idAuth);
            acceso.setRut(rut);
            acceso.setUuid(uuid);
            acceso.setDate(date);
            acceso.setName(name);
            acceso.setTextAuth(textAuth);
            acceso.setHash(hashSG400);

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

void SoapClient::error(struct soap *soap, QString WebServiceName, QMap<QString,QString> inputValues, QString response)
{
    bool state = soap_check_state(soap);

    QString mailText;
    mailText +="Error Web Service (" + WebServiceName + ") : \\n\\n Entrada : \\n";

    QMapIterator<QString, QString> i(inputValues);

    while (i.hasNext()) {
        i.next();
        mailText += "\\n- " + i.key().leftJustified(30,' ') + " : " + i.value();
    }

    mailText +="\\n\\n";
    QString error = "Error : ";

    if (state)
    {
        error += "Soap struct state not initialized";

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

        error += (soap->version ? "SOAP 1." : "Nb ") + (soap->version ? QString::number((int)soap->version) : QString::number(soap->error));
        error += "Fault   : " + QString::fromStdString(*fault);
        error += "subcode : [" + (subCodeFault ? QString::fromStdString(subCodeFault) : "no subcode") + "]";
        error += "reason  : [" + (reasonFault ? QString::fromStdString(reasonFault) : "no reason") + "]";
        error += "detail  : [" + (detailFault ? QString::fromStdString(detailFault) : "no detail") + "]";

    } else {

        error += "Repuesta Invalida";
    }

    qCritical() << error;
    mailText += error + "\\n";
    mailText += "\\n\\n Salida : \\n";
    mailText += "\""+ response +"\"";

    QStringList parameter;
    parameter << mailText;

    QProcess::startDetached("/var/lib/QProcess/emailWS.sh", parameter);
}

bool SoapClient::syncro(Persona *persona,Acceso *acceso, QDateTime dateTime)
{
    actionValidarCasino(persona,acceso,dateTime,true);
    return true;
}
