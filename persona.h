#ifndef PERSONA_H
#define PERSONA_H
#include <QTextStream>
#include <QStringList>

class Persona : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString rut READ rut WRITE setRut)
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid)
    Q_PROPERTY(QString hash READ hash WRITE setHash)
    Q_PROPERTY(int tipoMarca READ tipoMarca WRITE setTipoMarca)
    Q_PROPERTY(QByteArray foto READ foto WRITE setFoto)
    Q_PROPERTY(int fingerprintID READ fingerprintID WRITE setFingerprintID)

public:
    explicit Persona(QObject *parent = 0);

    QString rut() const;
    void setRut(const QString &);
    QString uuid() const;
    void setUuid(const QString &);
    QString hash() const;
    void setHash(const QString &);
    QString formated_rut() const;
    void setTipoMarca(int tipoMarca);
    int tipoMarca();
    void setFingerprintID(int fingerprintID);
    int fingerprintID();
    void setFoto(QByteArray foto);
    QByteArray foto();

    friend QTextStream & operator<<(QTextStream &stream, const Persona& persona);
    friend QTextStream & operator>>(QTextStream &stream, Persona& persona);

    enum MarcaType{
        MARCA_FINGER = 1,
        MARCA_RFID = 2,
        MARCA_INFO = 4
    };

private:
    QString m_rut;
    QString m_uuid;
    QString m_hash;
    QByteArray m_foto;
    int m_tipoMarca;
    int m_fingerprintID;
};

#endif // PERSONA_H
