//ifndef PRINTER_H

#include <qserialport.h>
#include <libusb.h>
#include <usbutil.h>
#include <QStringList>

#define CTL_LF "\x0a"
#define FEED_BEFORE_CUT "\x0a\x0a\x0a\x0a"
#define PAPER_FULL_CUT "\x1d\x56\x00"
#define PAPER_PART_CUT "\x1d\x56\x01"
#define TXT_BOLD_ON "\x1b\x45\x01"
#define TXT_ALIGN_CT "\x1b\x61\x01"
#define TXT_BOLD_OFF "\x1b\x45\x00"
#define CTL_CR "\x0d"
#define HW_INIT "\x1b\x40"
#define TXT_ALIGN_LT "\x1b\x61\x00"
#define CTL_RT "\x1b\x4a\x02"

class Printer
{
public:
    Printer(QString serialPort);
    Printer(const QString idVendor, const QString idProduct);
    bool print();
    bool printTitle(QString text);
    void setTitle(QString text);
    void setLine(QString text);
    bool cutPaper();

private:
    libusb_device_handle *usbDev;
    //QextSerialPort *serialPrinter;
    UsbUtil *usbPrinter;
    QStringList lines;
    QString idVendor;
    QString idProduct;
};

//endif // PRINTER_H


