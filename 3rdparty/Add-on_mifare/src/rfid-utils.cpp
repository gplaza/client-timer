#include "rfid-utils.h"

Rfid_Utils::Rfid_Utils()
{
}

QString Rfid_Utils::String_to_QStringDecimal(char* string)
{
    QString result = "";
    QString value = QString::fromLocal8Bit(string);
    bool ok;
    uint appId = value.toUInt(&ok,16);

    result = QString("%1").arg(appId,0,10);

    return result;
}

QString Rfid_Utils::block_to_DecimalQString(MifareClassicBlock data,int len)
{
    QString result;
    QString value = "";

    for(int i=0; i<len ;i++)
    {
        result.append(value.sprintf("%d",data[i]));
    }

    return result;
}

QString Rfid_Utils::block_to_QStringHex(MifareClassicBlock data,int len)
{
    QString result;
    QString value = "";

    for(int i=0; i<len ;i++)
    {
        result.append(value.sprintf("%02x",data[i]));
    }

    return result;
}


QString Rfid_Utils::ins_spaces(QString str) {
    //to be used with hex digits "ffffffff" becomes "ff ff ff ff"
    QString x=str;
    int i=2;
    for (;i<x.size();i=i+3) {
        x.insert(i,QString(" "));
    }
    return x;
}

QString Rfid_Utils::byte_to_QStringHex(byte_t* bytes, int len)
{
    // function to convert from byte_t to Qstring rt will hold data in Hex Format
    QString rt;
    for(int i=0; i<len; i++)
    {
        char l = (bytes[i]>>4)&0x0F;
        char r= bytes[i]&0x0F;

        unsigned char conv;
        unsigned char conv2;

        if(l<=9)
        {
            conv = l+0x30;
        }
        else
        {
            conv = l-0x0A+'A';
        }

        if(r<=9)
        {
            conv2= r+0x30;
        }
        else
        {
            conv2 = r-0x0A+'A';
        }

        QChar c[2] = { conv,conv2};
        rt += QString(c,2);
    }
    return  rt.toUpper();
}

byte_t * QString_to_Byte(QString data) {
    //  convert from QString to byte
    QString str = data;
    byte_t *s = NULL;
    s = (unsigned char*)qstrdup(str.toLatin1().constData());
    return s;
}

QString Rfid_Utils::a(byte_t* bytes,int len) {
    return ins_spaces(byte_to_QStringHex(bytes,len));
}

void Rfid_Utils::QStringHex_to_byte(QString hexs,byte_t* b)
{
    //VOID FUNCTION!!!declare byte array  b first!
    //converts an hex string "ffffffff" in is equivalent byte_t { '0xff','0xff','0xff','0xff' }
    QByteArray ba = hexs.toLatin1();
    const char *c = ba.data();
    char z[5],*f;
    unsigned int i=0;
    for (i=0;i<=strlen(c);i=i+2) {
        z[0]='0';
        z[1]='x';
        z[2]=c[i];
        z[3]=c[i+1];
        z[4]='\0';
        b[i/2]=strtol(z,&f,16);
    }
}
