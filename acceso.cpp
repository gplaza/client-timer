#include "acceso.h"

Acceso::Acceso(QObject *parent) : QObject(parent)
{
}

QString Acceso::toString() const
{
    return QString::number(m_idAuth) + ";" + textAuth() + ";" + m_rut + "-" + m_dv + ";" + m_date.toString("ddd MMM dd hh:mm:ss yyyy") + ";" + m_name + ";" + m_event;
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
    m_textAuth = t;
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
    m_rut = r;
}

QString Acceso::dv() const
{
    return m_dv;
}

void Acceso::setDv(const QString &d)
{
    m_dv = d;
}

QString Acceso::uuid() const
{
    return m_uuid;
}

void Acceso::setUuid(const QString &u)
{
    m_uuid = u;
}

QString Acceso::event() const
{
    return m_event;
}

void Acceso::setEvent(const QString &e)
{
    m_event = e;
}

QString Acceso::hash() const
{
    return m_hash;
}

void Acceso::setHash(const QString &h)
{
    m_hash = h;
}

Acceso::EventPrint Acceso::eventPrint() const
{
    return m_print;
}

void Acceso::setEventPrint(EventPrint eventprint)
{
    m_print = eventprint;
}

QString Acceso::complete_rut() const
{
    return m_rut + m_dv;
}
