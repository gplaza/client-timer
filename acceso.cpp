#include "acceso.h"

Acceso::Acceso(QObject *parent) : QObject(parent)
{
    QDateTime now = QDateTime::currentDateTime();

    this->m_idAuth = -1;
    this->m_uuid = "-";
    this->m_textAuth = "WebService Error";
    this->m_date = now;
    this->m_name = "";
    this->m_hash = ";";
    this->m_info_print = "";
    this->m_count_lunch = 0;
    this->m_count_dinner = 0;
}

QString Acceso::toString() const
{
    return QString::number(m_idAuth) + ";" + textAuth() + ";" + m_rut + ";" + m_date.toString("ddd MMM dd hh:mm:ss yyyy") + ";" + m_name;
}

QDateTime Acceso::date() const
{
    return m_date;
}

QString Acceso::dateFormated(const QString &format)
{
    return m_date.toString(format);
}

QString Acceso::rutFormated()
{
    return m_rut.mid(0,m_rut.length() - 7) + '.' + m_rut.mid(m_rut.length() - 7,3) + '.' + m_rut.mid(m_rut.length() - 4,3) +  "-" + m_rut.right(1);
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

int Acceso::count_casino()
{
    return m_count_casino;
}

void Acceso::setCount_casino(int i)
{
    m_count_casino = i;
}

int Acceso::count_lunch()
{
    return m_count_lunch;
}

void Acceso::setCount_lunch(int i)
{
    m_count_lunch = i;
}

int Acceso::count_dinner()
{
    return m_count_dinner;
}

void Acceso::setCount_dinner(int i)
{
    m_count_dinner = i;
}

QString Acceso::info_print() const
{
    return m_info_print;
}

void Acceso::setInfo_print(const QString &ip)
{
    m_info_print = ip;
}
