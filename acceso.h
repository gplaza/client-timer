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
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString hash READ hash WRITE setHash)
    Q_PROPERTY(int tipoAcceso READ tipoAcceso WRITE setTipoAcceso)

public:
    explicit Acceso(QObject *parent = 0);

    QString dateFormated(const QString &format);
    QString toString() const;

    QDateTime date() const;
    void setDate(const QDateTime &);

    QString textAuth() const;
    void setTextAuth(const QString &);
    QString name() const;
    void setName(const QString &);
    QString rut() const;
    void setRut(const QString &r);
    QString uuid() const;
    void setUuid(const QString &);
    QString hash() const;
    void setHash(const QString &);

    int idAuth();
    void setIdAuth(int);
    int tipoAcceso();
    void setTipoAcceso(int t);

    enum ResponseType {
        PERSON_OK = 0,
        PERSON_NO_EXIST = 7,
        PERSON_CRED_NO_EXIST = 24,
        PERSON_NO_LUNCH = 20,
        PERSON_NO_DINNER = 20,
        PERSON_SERVICE_USED = 23,
        OUT_OF_HOURS = 18,
        FINGERPRINT_NOT_MATCH = 19
    };

    // TODO : use database for this type ...
    enum AccesoType {
        TYPE_TARJETA = 1,
        TYPE_HUELLA_SG400 = 2
    };

signals:
    void finishedProcess();

private:
    QDateTime m_date;
    QString m_textAuth;
    QString m_rut;
    QString m_uuid;
    QString m_name;
    QString m_hash;

    int m_idAuth;
    int m_tipoAcceso;
    int m_tipoCredencial;
};

#endif // ACCESO_H
