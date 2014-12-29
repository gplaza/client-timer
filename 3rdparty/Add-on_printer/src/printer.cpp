#include "printer.h"

Printer::Printer(QString serialPort)
{
    usbDev = NULL;
    //QSerialPort serial(serialPort);

    /*serialPrinter = new QextSerialPort(serialPort);

    serialPrinter->setBaudRate(BAUD9600);
    serialPrinter->setFlowControl(FLOW_OFF);
    serialPrinter->setParity(PAR_NONE);
    serialPrinter->setDataBits(DATA_8);
    serialPrinter->setStopBits(STOP_1);
    serialPrinter->setTimeout(1);
    */
}

Printer::Printer(const QString idVendor, const QString idProduct)
{
    this->idProduct = idProduct;
    this->idVendor = idVendor;
    int init;

    init = usbPrinter->dev_find(idVendor,idProduct,&usbDev);

    if(init < 0)
    {
        usbDev = NULL;
        qCritical() << "Error USB printer";
    }
}

bool Printer::printTitle(QString text)
{
    usbPrinter->dev_open(usbDev);

    text = text + "\x0A\x1D\x49\x01\x0A";
    int sizeOutput = text.length();
    usbPrinter->dev_write(usbDev,text,sizeOutput);

    return true;
}

void Printer::setTitle(QString text)
{
    QString bold(TXT_BOLD_ON);
    QString align_center(TXT_ALIGN_CT);

    lines.append(bold + align_center + text);
}

void Printer::setLine(QString text)
{
    lines.append(text);
}

bool Printer::print()
{
    QString line;

    if(usbDev != NULL)
    {
        usbPrinter->dev_open(usbDev);

        usbPrinter->dev_write(usbDev,HW_INIT,2);

        for (int i = 0; i < lines.size(); ++i)
        {
            int len = lines.at(i).length();
            line = lines.at(i) + "\x0A";
            usbPrinter->dev_write(usbDev,line,len + 1);
        }

        this->cutPaper();
        usbPrinter->dev_close(usbDev);
        lines.clear();
    }

    /*
    if(serialPrinter != NULL)
    {
        bool res = false;
        res = serialPrinter->open(QIODevice::ReadWrite);

        if(res)
        {
            serialPrinter->write(HW_INIT);

            for (int i = 0; i < lines.size(); ++i)
            {
                line = lines.at(i) + "\x0A";
                serialPrinter->write(line.toLocal8Bit());
            }

            this->cutPaper();
            serialPrinter->close();
            lines.clear();
        }
    }
    */

    return true;
}

bool Printer::cutPaper()
{

    if(usbDev != NULL)
    {
        usbPrinter->dev_write(usbDev,FEED_BEFORE_CUT,4);
        usbPrinter->dev_write(usbDev,PAPER_PART_CUT,4);
    }

    /*
    if(serialPrinter != NULL)
    {
        bool res = false;
        res = serialPrinter->open(QIODevice::ReadWrite);

        if(res)
        {
            serialPrinter->write(QByteArray(FEED_BEFORE_CUT));
            serialPrinter->write(QByteArray(PAPER_PART_CUT));
            serialPrinter->close();
        }
    }
    */


    return true;
}

