#include "fotoprovider.h"

FotoProvider::FotoProvider()
    : QQuickImageProvider(QQmlImageProviderBase::Image)
{
}


FotoProvider::~FotoProvider()
{

}

QImage FotoProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QString rut = id.contains(".new")? id.mid(0,id.length()-4) : id;

    Q_UNUSED(size);
    Q_UNUSED(requestedSize);

    QByteArray img = Bdd::getImage(rut);

    QImage result;
    if(!img.isEmpty())
        result.loadFromData(img,"JPEG");
    else
        result.load(":/qml/images/unknown.jpg","JPEG");

    return result;
}

