//ifndef IDISPLAY_H
#include <QDebug>

static unsigned char cubeChar [8] =
{
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
};

class IDisplay : public QObject
{
    Q_OBJECT

public :
    virtual ~IDisplay() {}
    virtual void message(QString message, const int x, const int y, const bool clear) = 0;
    virtual void blinkMessage(QString message) = 0;
    virtual void init(int rows, int cols) = 0;
    virtual void clear() = 0;

    enum SpecialChar{
        CUBE_CHAR = 2
    };
};

Q_DECLARE_INTERFACE (IDisplay,"cl.gplaza.IDisplay/1.0")

//endif // IDISPLAY_H


