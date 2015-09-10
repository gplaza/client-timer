#include "persona.h"

Persona::Persona(QObject *parent) : QObject(parent)
{
    m_foto = QByteArray();
    m_rut = "";
}

QString Persona::rutFormated()
{
    return m_rut.mid(0,m_rut.length() - 7) + '.' + m_rut.mid(m_rut.length() - 7,3) + '.' + m_rut.mid(m_rut.length() - 4,3) +  "-" + m_rut.right(1);
}

QString Persona::rut() const
{
    return m_rut;
}

void Persona::setRut(const QString &r)
{
    m_rut = r;
}

QString Persona::uuid() const
{
    return m_uuid;
}

void Persona::setUuid(const QString &u)
{
    m_uuid = u;
}

QString Persona::hash() const
{
    return m_hash;
}

void Persona::setHash(const QString &h)
{
    m_hash = h;
}

void Persona::setTipoMarca(int tipoMarca)
{
    m_tipoMarca = tipoMarca;
}

int Persona::tipoMarca()
{
    return m_tipoMarca;
}

void Persona::setFingerprintID(int fingerprintID)
{
    m_fingerprintID = fingerprintID;
}

int Persona::fingerprintID()
{
    return m_fingerprintID;
}

void Persona::setFoto(QByteArray foto)
{
    m_foto = foto;
}

QByteArray Persona::foto()
{
    return m_foto;
}

void Persona::setExistFoto(bool b)
{
    m_existFoto = b;
}

bool Persona::existFoto()
{
    return m_existFoto;
}


QTextStream& operator<< (QTextStream &stream, const Persona &persona)
{
    QStringList sl;

    sl << persona.rut() << persona.uuid();
    stream << sl.join(";") << "\n";

    return stream;
}

QTextStream& operator>> (QTextStream &stream, Persona &persona)
{
    QString line = stream.readLine();
    QStringList fields = line.split(";");

    persona.setRut(fields[0]);
    persona.setUuid(fields[2]);

    return stream;
}

