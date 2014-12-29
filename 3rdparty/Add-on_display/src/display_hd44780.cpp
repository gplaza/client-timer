#include "display_hd44780.h"
#include <unistd.h>

Display_HD44780::Display_HD44780() : IDisplay()
{
}

void Display_HD44780::init(int rows, int cols)
{
    wiringPiSetup();

    // Number of lines : 2
    // Number of chararacter : 16
    // Bus bit : 4 (bit)
    // RS : 2 (GPIO21 (PCM_DOUT))
    // E  : 0 (GPIO17)
    // D4 : 6  (GPIO25)
    // D5 : 5  (GPIO24)
    // D6 : 4  (GPIO23)
    // D7 : 3  (GPIO22)
    lcdHandle = lcdInit(rows,cols,4,2,0,6,5,4,3,0,0,0,0);
    lcdCharDef(lcdHandle,Display_HD44780::CUBE_CHAR , cubeChar);

    if (lcdHandle < 0)
        qDebug() << "lcdInit failed";

}

void Display_HD44780::clear()
{
    lcdClear(lcdHandle);
}

void Display_HD44780::insertSpecialChar(int index)
{
    lcdPutchar(lcdHandle,index) ;
}

void Display_HD44780::blinkMessage(QString message)
{
    lcdClear(lcdHandle);

    for(int i=0; i<= 2 ; i++)
    {
        lcdPosition(lcdHandle,0,0);
        lcdPuts(lcdHandle,message.toLatin1().data());
        delay(500);
        lcdClear(lcdHandle);
        delay(500);
    }

    lcdPosition(lcdHandle,0,0);
    lcdPuts(lcdHandle,message.toLatin1().data());
}

void Display_HD44780::message(QString message, const int x, const int y, const bool clear)
{
    if(clear)
        lcdClear(lcdHandle);

    lcdPosition(lcdHandle,x,y);
    lcdPuts(lcdHandle,message.toLatin1().data());
}

void Display_HD44780::scrollMessage(QString message)
{
    message = "                " + message + "                 ";
    QByteArray b_message = message.toLocal8Bit();
    const char *c_message = b_message.constData();

    for (;;)
    {
        char buf [32] ;
        static int position = 0 ;

        delay(250);
        strncpy (buf, &c_message[position], 16) ;
        buf[16] = 0 ;
        lcdPosition (lcdHandle, 0, 0) ;
        lcdPuts(lcdHandle, buf) ;

        if (++position == (message.length() - 16))
            return;
    }
}


