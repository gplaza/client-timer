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
    QByteArray img = Bdd::getImage(id);
    QImage result;
    result.loadFromData(img,"JPEG");

    return result;
}

