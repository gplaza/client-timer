#include "acceso.h"
#include <QUuid>

Acceso::Acceso(QObject *parent) : QObject(parent)
{
    QDateTime now = QDateTime::currentDateTime();

    this->m_idAuth = -1;
    this->m_uuid = "-";
    this->m_textAuth = "WebService Error";
    this->m_date = now;
    this->m_name = "";
    this->m_hash = ";";
}

QString Acceso::toString() const
{
    return QString::number(m_idAuth)  + ";" + m_rut + ";" + m_name;
}

QDateTime Acceso::date() const
{
    return m_date;
}

QString Acceso::dateFormated(const QString &format)
{
    return m_date.toString(format);
}

void Acceso::setDate(const QDateTime &d)
{
    m_date = d;
}

int Acceso::idAuth()
{
    return m_idAuth;
}

void Acceso::setIdAuth(int i)
{
    m_idAuth = i;
}

QString Acceso::textAuth() const
{
    return m_textAuth;
}

void Acceso::setTextAuth(const QString &t)
{
    if (t != m_textAuth) {
        m_textAuth = t;
    }
}

QString Acceso::name() const
{
    return m_name;
}

void Acceso::setName(const QString &n)
{
    m_name = n;
}

QString Acceso::rut() const
{
    return m_rut;
}

void Acceso::setRut(const QString &r)
{
    if (r != m_rut) {
        m_rut = r;
    }
}

QString Acceso::uuid() const
{
    return m_uuid;
}

void Acceso::setUuid(const QString &u)
{
    m_uuid = u;
}

QString Acceso::hash() const
{
    return m_hash;
}

void Acceso::setHash(const QString &h)
{
    m_hash = h;
}

int Acceso::tipoAcceso()
{
    return m_tipoAcceso;
}

void Acceso::setTipoAcceso(int t)
{
    m_tipoAcceso = t;
}
