#ifndef FOTOPROVIDER_H
#define FOTOPROVIDER_H

#include <QQuickImageProvider>
#include <bdd.h>

class FotoProvider : public QQuickImageProvider
{
public:
    FotoProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
    ~FotoProvider();
};

#endif // FOTOPROVIDER_H
