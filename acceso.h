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
    Q_PROPERTY(int count_casino READ count_casino WRITE setCount_casino)
    Q_PROPERTY(int count_lunch READ count_lunch WRITE setCount_lunch)
    Q_PROPERTY(int count_dinner READ count_dinner WRITE setCount_dinner)
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
    int count_casino();
    void setCount_casino(int);
    int count_lunch();
    void setCount_lunch(int);
    int count_dinner();
    void setCount_dinner(int);

    QString complete_rut() const;

    /*
      0 : Registro correcto
      1 : Persona ya enrolada
      2 : Persona no enrolada
      3 : Error en base de datos CA
      4 : Error en base de datos Tempo
      5 : Registra evento
      6 : Formato invalido (Hash | Credencial | RFID)
      7 : Persona no existe
      8 : Persona no ingresada en Tempo
      9 : Persona no ingresada en CA
     10 : Contrato vencido
     11 : No registrado en punto control
     12 : Fuera del horario
     13 : Credencial /huella Inhabilitada
     14 : Formato tipo marca inválida
     15 : Sin imagen huella
     16 : No quedan almuerzos
     17 : No quedan cenas
     18 : Fuera de horario
     19 : No tiene acceso
     20 : No le quedan almuerzo
     21 : No registra asistencia
     22 : Error en base de datos As400
     23 : Ya uso el servicio de alimentación
     24 : Persona no existe en AS400
    */

    enum ResponseType{
        PERSON_OK = 0,
        PERSON_NO_EXIST = 7,
        PERSON_NO_LUNCH = 20
    };

private:
    QDateTime m_date;
    int m_idAuth;
    QString m_textAuth;
    QString m_rut;
    QString m_dv;
    QString m_uuid;
    QString m_name;
    QString m_hash;

    int m_count_casino;
    int m_count_lunch;
    int m_count_dinner;
};

#endif // ACCESO_H
