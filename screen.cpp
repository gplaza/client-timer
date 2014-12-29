#include "screen.h"
#include <QtNetwork/QNetworkInterface>
#include <QStringList>

Screen::Screen(QObject *parent) : QObject(parent)
{
    qDebug() << "init LCD";

    display.init(2,16);
    online = true;
    display.message("Hello",true);
    showTime();
    connect(&timer, SIGNAL(timeout()), this, SLOT(showTime()));
}

void Screen::showMessage(const QString &m)
{
    timer.stop();
    display.clear();
    display.message(m);
    QTimer::singleShot(1000, this, SLOT(showTime()));
}

void Screen::showWaitMessage(const QString &m)
{
    timer.stop();
    display.blinkMessage(m);
}

void Screen::onLine()
{
    online = true;
}

void Screen::offLine()
{
    online = false;
}

void Screen::progressBar(const QString &m, int percentage)
{
    timer.stop();

    QString spacer = "";
    QString ps = QString::number(percentage) + "%";
    int i = 0;
    int p = percentage/10;

    display.message(m,0,0,percentage == 0 ? true : false);
    display.message("[",0,1,false);

    for (i = 0; i< p ;i++)
        display.insertSpecialChar(IDisplay::CUBE_CHAR);
    for (i = p; i< 10 ;i++)
        spacer += " ";

    display.message(spacer + "]" + ps.rightJustified(4,' '),1 + p,1,false);
}

void Screen::showTime()
{
    timer.start(1000);

    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    QString currentActiveConn = "";
    QString currentLineState = (online)? "Online" : "OffLine";
    QStringList activeConn;

    foreach(const QNetworkInterface & interface, interfaces)
    {
        if ((interface.flags() & QNetworkInterface::IsUp) && (interface.flags() & QNetworkInterface::IsRunning))
        {
            if(interface.name() == "eth0")
                activeConn << "lan";
            if(interface.name() == "wlan0")
                activeConn << "wifi";
        }
    }

    currentActiveConn = (activeConn.empty())? currentActiveConn.fill(' ', 16) : activeConn.join("/").leftJustified(16, ' ');

    display.message(currentLineState.leftJustified(8,' ') + QTime::currentTime().toString("hh:mm:ss"),0,0,false);
    display.message(currentActiveConn,0,1,false);
}
