#include "display_i2c.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <QDebug>

// pcf8574 bit positions of the lcd control pins
// bits 4 to 7 are the data bits
#define LCD_I2C_RS 0
#define LCD_I2C_RW 1
#define LCD_I2C_E  2
#define LCD_I2C_BACKLIGHT 3
#define	LCD_CGRAM	0x40

// convenience macros
// write current value of output variable to device
#define LCD_I2C_WRITE(lcd_p) wiringPiI2CWrite(lcd_p->fd, lcd_p->output);
// enable
#define LCD_I2C_E_HI(lcd_p)  lcd_p->output |= (1<<LCD_I2C_E)
#define LCD_I2C_E_LO(lcd_p)  lcd_p->output &=~ (1<<LCD_I2C_E)
// set data or instruction mode
#define LCD_I2C_RS_D(lcd_p) lcd_p->output |= (1<<LCD_I2C_RS)
#define LCD_I2C_RS_I(lcd_p) lcd_p->output &=~ (1<<LCD_I2C_RS)
// set the data nibble -
// note: will need to change this for devices with different wiring.
#define LCD_I2C_DATA_NIBBLE(lcd_p, x) lcd_p->output= (x<<4 | (lcd_p->output & 0x0f))
//! set the lcd data ram address to that passed, only lower 7 bits are used.
#define LCD_I2C_SET_DD_RAM_ADDRESS(lcd_p, address) lcd_i2c_write_i(lcd_p, 0x80 | ((address) & 0x7f) )

DisplayI2C::DisplayI2C() : IDisplay()
{    
}

void DisplayI2C::init(int rows, int cols)
{
    lcd={0};

    int lcdHandle = lcd_i2c_setup(&lcd,rows,cols, LCD_I2C_PCF8574_ADDRESS_DEFAULT);

    if (lcdHandle == -1)
        qDebug() << "lcdInit failed";

    lcd_i2c_init(&lcd);
    lcd_i2c_backlight(&lcd,1);
    lcdCharDef(&lcd,CUBE_CHAR , cubeChar);
}

void DisplayI2C::clear()
{
    lcd_i2c_clear(&lcd);
    delay(500);
}

void DisplayI2C::blinkMessage(QString message)
{
    lcd_i2c_clear(&lcd);

    for(int i=0; i<= 2 ; i++)
    {
        lcd_i2c_gotoxy(&lcd,0,0);
        lcd_i2c_puts(&lcd,message.toLatin1().data());
        delay(500);
        lcd_i2c_clear(&lcd);
        delay(500);
    }

    lcd_i2c_gotoxy(&lcd,0,0);
    lcd_i2c_puts(&lcd,message.toLatin1().data());
}

void DisplayI2C::message(QString message, const int x, const int y, const bool clear)
{
    if(clear)
        lcd_i2c_clear(&lcd);

    lcd_i2c_gotoxy(&lcd,x,y);

    const char* msg = message.toStdString().c_str();
    lcd_i2c_puts(&lcd,msg);
}

// -----------------------------------------------------
// static functions
// -----------------------------------------------------
void DisplayI2C::lcd_i2c_e_assert(lcd_i2c_t *lcd)
{
    LCD_I2C_E_LO(lcd);
    LCD_I2C_WRITE(lcd);
    LCD_I2C_E_HI(lcd);
    LCD_I2C_WRITE(lcd);
    LCD_I2C_E_LO(lcd);
    LCD_I2C_WRITE(lcd);
}

//! Write passed data to device
//! Note: RS must be set prior to calling
void DisplayI2C::lcd_i2c_write(lcd_i2c_t *lcd, uint8_t data)
{
    // high nibble
    LCD_I2C_DATA_NIBBLE(lcd, (data >> 4));
    lcd_i2c_e_assert(lcd);
    // low nibble
    LCD_I2C_DATA_NIBBLE(lcd, (data & 0x0f));
    lcd_i2c_e_assert(lcd);
    // delay for command to be executed
    delayMicroseconds(50);
}

// -----------------------------------------------------
// user callable functions
// -----------------------------------------------------
void DisplayI2C::lcd_i2c_gotoxy(lcd_i2c_t *lcd, uint8_t x, uint8_t y)
{
    // note: on two line devices, second line begins at address 0x40
    // XXX so this will work with 1 and 2 line devices, but probabaly not with 4.
    LCD_I2C_SET_DD_RAM_ADDRESS( lcd,  y*0x40 + x  );
    lcd->x=x;
    lcd->y=y;
}

void DisplayI2C::lcd_i2c_printf(lcd_i2c_t *lcd,  char* format, ...)
{
    va_list args;
    char *spp=NULL;

    va_start(args, format);
    int r=vasprintf(&spp, format, args);
    if(r!=-1){
        lcd_i2c_puts(lcd, spp);
        free(spp);
    }
    va_end(args);
}

int DisplayI2C::lcd_i2c_setup( lcd_i2c_t *lcd,int rows, int cols, int address)
{
    lcd->output=0;
    lcd->fd=wiringPiI2CSetup(address);
    lcd->rows = rows;
    lcd->cols = cols;

    return lcd->fd;
}

void DisplayI2C::lcd_i2c_write_i(lcd_i2c_t *lcd, uint8_t data)
{
    LCD_I2C_RS_I(lcd);
    lcd_i2c_write(lcd,data);
}

void DisplayI2C::insertSpecialChar(int index)
{
    lcd_i2c_putc(&lcd,index);
}

void DisplayI2C::lcdCharDef(lcd_i2c_t *lcd,int index, unsigned char data [8])
{
    lcd_i2c_write_i(lcd, LCD_CGRAM | ((index & 7) << 3));
    int i = 0;

    for (i = 0 ; i < 8 ; ++i)
      lcd_i2c_write (lcd, data [i]) ;
}

void DisplayI2C::lcd_i2c_write_d(lcd_i2c_t *lcd, uint8_t data)
{
    LCD_I2C_RS_D(lcd);
    lcd_i2c_write(lcd,data);
}

void DisplayI2C::lcd_i2c_clear(lcd_i2c_t *lcd)
{
    lcd_i2c_write_i(lcd, 0x1);
    delay(2); // delay for command to take effect
}

void DisplayI2C::lcd_i2c_home(lcd_i2c_t *lcd)
{
    lcd_i2c_write_i(lcd, 0x02);
    lcd->x=0;
    lcd->y=0;
    delay(2); // delay for command to take effect
}

void DisplayI2C::lcd_i2c_putc(lcd_i2c_t *lcd, char c)
{
    lcd_i2c_write_d(lcd, c);

    if(++lcd->x == lcd->cols){
        lcd->x=0;
        if(++lcd->y == lcd->rows){
            lcd->y=0;
        }
        lcd_i2c_gotoxy(lcd,lcd->x,lcd->y);
    }
}

void DisplayI2C::lcd_i2c_puts(lcd_i2c_t *lcd, const char* str)
{
    while(*str!='\x0'){
        lcd_i2c_putc(lcd,*str++);
    }
}

void DisplayI2C::lcd_i2c_init(lcd_i2c_t *lcd)
{
    LCD_I2C_E_LO(lcd);
    LCD_I2C_RS_I(lcd);
    LCD_I2C_WRITE(lcd);

    // software reset
    LCD_I2C_DATA_NIBBLE(lcd,0x3);
    lcd_i2c_e_assert(lcd);
    delay(5); // ms
    lcd_i2c_e_assert(lcd);
    delayMicroseconds(150);
    lcd_i2c_e_assert(lcd);
    delayMicroseconds(150);

    // set 4 bit mode
    LCD_I2C_DATA_NIBBLE(lcd,0x02);
    lcd_i2c_e_assert(lcd);

    // entry mode - 0x06 is display shift on, increment address counter
    lcd_i2c_write_i(lcd,0x06);

    // set cursor
    LCD_I2C_CURSOR_BLINK_OFF(lcd);
    LCD_I2C_CURSOR_OFF(lcd);

    // clear and home
    lcd_i2c_clear(lcd);
    lcd_i2c_home(lcd);

}

/**
 * Turn backlight on or off
 * @param ct lcd_i2c_t control structure for the device
 * @param on Turn on if non-zero, off otherwise
 */
void DisplayI2C::lcd_i2c_backlight(lcd_i2c_t *lcd, uint8_t on)
{
    if(on)
        lcd->output |= (1<<LCD_I2C_BACKLIGHT);
    else
        lcd->output &=~ (1<<LCD_I2C_BACKLIGHT);
    LCD_I2C_WRITE(lcd);
}

