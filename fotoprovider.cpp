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
    Q_UNUSED(size);
    Q_UNUSED(requestedSize);

    QByteArray img = Bdd::getImage(id);

    QImage result;
    if(!img.isEmpty())
        result.loadFromData(img,"JPEG");
    else
        result.load(QCoreApplication::applicationDirPath() + "/qml/images/unknown.jpg","JPEG");

    return result;
}

