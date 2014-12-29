#ifndef SCREEN_H
#define SCREEN_H

#include <display_i2c.h>
#include <QTimer>
#include "QTime"

class Screen : public QObject
{
    Q_OBJECT
public:
    explicit Screen(QObject *parent = 0);

private:
    QTimer timer;
    DisplayI2C display;
    bool online;

public slots:
    void showMessage(const QString &m);
    void showWaitMessage(const QString &m);
    void progressBar(const QString &m, int percentage);
    void showTime();
    void offLine();
    void onLine();
};

#endif // SCREEN_H
