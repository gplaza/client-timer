#include "configuratoradapter.h"

ConfiguratorAdapter::ConfiguratorAdapter(QObject *parent) : QObject(parent)
{
    QObject::connect(Configurator::instance(),&Configurator::callServer,this,&ConfiguratorAdapter::receiveServerRequest);
}

void ConfiguratorAdapter::receiveServerRequest(QHttpRequest* req, QHttpResponse *resp)
{
    QRegExp *exp = new QRegExp();
    this->resp = resp;
    exp->setPattern("^/api/persona");
    qDebug() << "Adapter Api ...";

    if (exp->indexIn(req->path()) != -1 && req->method() == QHttpRequest::HTTP_POST) // Add new user
    {
        LOG_INFO("Persona function API (POST)");
        connect(req,&QHttpRequest::data,this,&ConfiguratorAdapter::addPersona);
    }

    if (exp->indexIn(req->path()) != -1 && req->method() == QHttpRequest::HTTP_PUT) // Update user
    {
        LOG_INFO("Persona function API (PUT)");
        connect(req,&QHttpRequest::data,this,&ConfiguratorAdapter::updatePersona);
    }

    if (exp->indexIn(req->path()) != -1 && req->method() == QHttpRequest::HTTP_DELETE) // Delete user
    {
        QUrlQuery query(req->url());

        if(query.hasQueryItem("userID"))
        {
            int id = query.queryItemValue("userID").toInt();
            emit deleteFingerPrint(id);

            LOG_INFO("Persona function API (DELETE)");
            qDebug() << "userID" << id;
        }
    }

}

void ConfiguratorAdapter::response(const QString &status, const QString &response)
{
    QString jsonState = "\"status\":\"" + status + "\"";
    QString json = (status == "success")? "{" + jsonState + ",\"id_huella\":" + response + "}" : "{" + jsonState + ",\"msg\":\"" + response + "\"}";
    QByteArray body = json.toLocal8Bit();

    if(status == "success")
        emit userChanged();

    if(resp)
    {
        qDebug() << "Emit response : " << json;
        resp->setHeader("Content-Length", QString::number(body.size()));
        resp->writeHead(200);
        resp->end(body);
        return;

    } else {

        qDebug() << "Error on response ...";
    }
}

void ConfiguratorAdapter::addPersona(const QByteArray &jsonValue)
{
    QJsonDocument json = QJsonDocument::fromJson(jsonValue);
    QJsonObject qJsonObject = json.object();

    QString hash = qJsonObject["hash"].toString();
    int typeHash = qJsonObject["typeHash"].toInt();

    emit registerFingerPrint(hash,typeHash);
}

void ConfiguratorAdapter::updatePersona(const QByteArray &jsonValue)
{
    QJsonDocument json = QJsonDocument::fromJson(jsonValue);
    QJsonObject qJsonObject = json.object();

    QString hash = qJsonObject["hash"].toString();
    QString id = qJsonObject["userID"].toString();
    int typeHash = qJsonObject["typeHash"].toInt();

    int userID = id.toInt();
    emit updateFingerPrint(hash,typeHash,userID);
}
