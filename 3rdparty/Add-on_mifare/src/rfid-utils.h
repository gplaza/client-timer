#ifndef UTILS_H
#define UTILS_H
#include <QtCore>
#include <freefare.h>

typedef uint8_t byte_t;

class Rfid_Utils
{
public:
    Rfid_Utils();

    QString ins_spaces(QString str);
    QString a(byte_t* bytes,int len);
    void QStringHex_to_byte(QString hexs,byte_t* b);
    QString byte_to_QStringHex(byte_t* bytes, int len);
    static QString block_to_DecimalQString(MifareClassicBlock data, int len);
    static QString block_to_QStringHex(MifareClassicBlock data, int len);
    static QString String_to_QStringDecimal(char* string);
};

#endif // UTILS_H
