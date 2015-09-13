#include "acceso.h"
#include <QUuid>

Acceso::Acceso(const QString &casinoName, QObject *parent) : m_casinoName(casinoName), QObject(parent)
{
    QDateTime now = QDateTime::currentDateTime();

    this->m_idAuth = -1;
    this->m_uuid = "-";
    this->m_textAuth = "WebService Error";
    this->m_date = now;
    this->m_name = "";
    this->m_hash = ";";
    this->m_count_casino = 0;
    this->m_count_lunch = 0;
    this->m_count_dinner = 0;
}

QString Acceso::toString() const
{
    return QString::number(m_idAuth) + ";" + textAuth() + ";" +
            QString::number(m_count_casino) +  ";" + QString::number(m_count_lunch) + ";" + QString::number(m_count_dinner) + ";" +
            m_rut + ";" + m_name;
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
        emit textAuthChanged();
    }
}

QString Acceso::name() const
{
    return m_name;
}

void Acceso::setName(const QString &n)
{
    if (n != m_name) {
        m_name = n;
        emit nameChanged();
    }
}


QString Acceso::casinoName() const
{
    return m_casinoName;
}

void Acceso::setCasinoName(const QString &cn)
{
    if (cn != m_casinoName) {
        m_casinoName = cn;
        emit casinoNameChanged();
    }
}

QString Acceso::rut() const
{
    return m_rut;
}

void Acceso::setRut(const QString &r)
{
    if (r != m_rut) {
        m_rut = r;
        this->setRutFormat(r);
        emit rutChanged();
    }
}

QString Acceso::rutFormat() const
{
    return m_rutFormat;
}

void Acceso::setRutFormat(const QString &r)
{
    QString formated = r.mid(0,r.length() - 7) + '.' + r.mid(r.length() - 7,3) + '.' + r.mid(r.length() - 4,3) +  "-" + r.right(1);

    if (formated != m_rutFormat) {
        m_rutFormat = formated;
        emit rutFormatChanged();
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

int Acceso::count_casino()
{
    return m_count_casino;
}

void Acceso::setCount_casino(int i)
{
    if (i != m_count_casino && i!= 0) {
        m_count_casino = i;
        emit count_casinoChanged();
    }
}

int Acceso::count_lunch()
{
    return m_count_lunch;
}

void Acceso::setCount_lunch(int i)
{
    if (i != m_count_lunch) {
        m_count_lunch = i;
        emit count_lunchChanged();
    }
}

int Acceso::count_dinner()
{
    return m_count_dinner;
}

void Acceso::setCount_dinner(int i)
{
    if (i != m_count_dinner) {
        m_count_dinner = i;
        emit count_dinnerChanged();
    }
}
