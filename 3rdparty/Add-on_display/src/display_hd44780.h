#ifndef DISPLAY_HD44780_H
#define DISPLAY_HD44780_H
#include <wiringPi.h>
#include <idisplay.h>
#include <lcd.h>
#include <QDebug>
#include <QObject>

class Display_HD44780 : public IDisplay
{
public:
    Display_HD44780();
    void init(int rows, int cols);
    void clear();
    void scrollMessage(QString message);
    void blinkMessage(QString message);
    void insertSpecialChar(int index);
    void message(QString message, const int x=0, const int y=0,const bool clear=true);

private:
    int lcdHandle;
};

#endif // DISPLAY_HD44780_H
