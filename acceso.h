#ifndef ACCESO_H
#define ACCESO_H
#include <QDateTime>

class Acceso : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDateTime date READ date WRITE setDate)
    Q_PROPERTY(int idAuth READ idAuth WRITE setIdAuth)
    Q_PROPERTY(QString textAuth READ textAuth WRITE setTextAuth)
    Q_PROPERTY(QString rut READ rut WRITE setRut)
    Q_PROPERTY(QString dv READ dv WRITE setDv)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString hash READ hash WRITE setHash)
    Q_ENUMS(EventPrint)

public:
    explicit Acceso(QObject *parent = 0);

    QString dateFormated(const QString &format);
    QString toString() const;

    QDateTime date() const;
    void setDate(const QDateTime &);
    int idAuth();
    void setIdAuth(int);
    QString textAuth() const;
    void setTextAuth(const QString &);
    QString name() const;
    void setName(const QString &);
    QString rut() const;
    void setRut(const QString &);
    QString dv() const;
    void setDv(const QString &);
    QString uuid() const;
    void setUuid(const QString &);
    QString hash() const;
    void setHash(const QString &);

    /*
    0;registro correcto;rut; fecha;Datos personales; Entrada|salida|evento ;
    1;Persona ya enrolada;rut;fecha;Datos personales;Persona valida;
    1;Persona no enrolada;rut;fecha;Datos personales;Persona valida;
    1; Error en base de datos;rut;fecha; Sin informacion;Modulo principal CA;
    2; Error en base de datos;rut;fecha; Sin informacion; Tempo;
    3; Registra evento;rut;fecha; Datos personales; Evento;
    4; Formato Invalido;rut;fecha; Datos personales; hash|credencial|RFID;
    5;Persona no existe;rut;fecha; ; Sin informacion; Individuo;
    6;Persona no ingresada en sistema;rut;fecha; ; Sin informacion; Tempo;
    6;Persona no ingresada en sistema;rut;fecha; ; Sin informacion; dato;
    7;Contrato vencido;rut;fecha; Datos personales; fecha contrato;
    8;No registrado en punto control;rut;fecha; Datos personales; punto control;
    9;Fuera del horario;rut;fecha; Datos personales; bloque horario asignado;
    */

    enum ResponseType{
        PERSON_OK = 0,
        PERSON_FINGERPRINT_STATE = 1,
        PERSON_REGISTER_EVENT = 3,
        PERSON_NO_EXIST = 5,
        PERSON_NO_REGISTERED = 6,
        PERSON_HOUR_ERROR = 9,
        PERSON_NO_REGISTERED_IN_AREA = 8,
        PERSON_EXPIRED = 7
    };

    QString complete_rut() const;

private:
    QDateTime m_date;
    int m_idAuth;
    QString m_textAuth;
    QString m_rut;
    QString m_dv;
    QString m_uuid;
    QString m_name;
    QString m_hash;
};

#endif // ACCESO_H
