#ifndef ACCESO_H
#define ACCESO_H
#include <QDateTime>

class Acceso : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDateTime date READ date WRITE setDate)
    Q_PROPERTY(int idAuth READ idAuth WRITE setIdAuth)
    Q_PROPERTY(QString textAuth READ textAuth WRITE setTextAuth NOTIFY textAuthChanged)
    Q_PROPERTY(QString rut READ rut WRITE setRut NOTIFY rutChanged)
    Q_PROPERTY(QString rutFormat READ rutFormat WRITE setRutFormat NOTIFY rutFormatChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString hash READ hash WRITE setHash)
    Q_PROPERTY(int count_casino READ count_casino WRITE setCount_casino NOTIFY count_casinoChanged)
    Q_PROPERTY(int count_lunch READ count_lunch WRITE setCount_lunch NOTIFY count_lunchChanged)
    Q_PROPERTY(int count_dinner READ count_dinner WRITE setCount_dinner NOTIFY count_dinnerChanged)
    Q_PROPERTY(QString casinoName READ casinoName WRITE setCasinoName NOTIFY casinoNameChanged)

public:
    explicit Acceso(const QString &casinoName = "", QObject *parent = 0);

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
    void setRut(const QString &r);
    QString rutFormat() const;
    void setRutFormat(const QString &r);
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
    void setTimeShow(int);
    int timeShow();
    QString casinoName() const;
    void setCasinoName(const QString &);
    QString info_print() const;
    void setInfo_print(const QString &);
    QString beca_print() const;
    void setBeca_print(const QString &);

    enum ResponseType{
        PERSON_OK = 0,
        PERSON_NO_EXIST = 7,
        PERSON_CRED_NO_EXIST = 24,
        PERSON_NO_LUNCH = 20,
        PERSON_NO_DINNER = 20,
        PERSON_SERVICE_USED = 23,
        OUT_OF_HOURS = 18
    };

signals:
    void textAuthChanged();
    void rutChanged();
    void rutFormatChanged();
    void nameChanged();
    void count_casinoChanged();
    void count_lunchChanged();
    void count_dinnerChanged();
    void timeShowChanged();
    void casinoNameChanged();
    void finishedProcess();

private:
    QDateTime m_date;
    int m_idAuth;
    QString m_textAuth;
    QString m_rut;
    QString m_rutFormat;
    QString m_uuid;
    QString m_name;
    QString m_hash;
    QString m_casinoName;
    int m_count_casino;
    int m_count_lunch;
    int m_count_dinner;

};

#endif // ACCESO_H
