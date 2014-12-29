#include "usbutil.h"

UsbUtil::UsbUtil()
{
}

int UsbUtil::dev_find(QString vendor_id, QString product_id, libusb_device_handle **handle)
{
    int r =0, i = 0;
    struct libusb_device **devs;
    struct libusb_device *dev;
    struct libusb_device_handle *_handle;
    QString value;

    r = libusb_init(NULL);

    if(r < 0)
    {
        qCritical() << "failed to initialise libusb";
        return -1;
    }

    qDebug() << "Init libusb";

    r = libusb_get_device_list(NULL, &devs);

    if (r < 0)
    {
        qCritical() << "There are no USB devices on bus";
        return -1;
    }

    qDebug() << "Device USB Count : " << r ;

    while ((dev = devs[i++]) != NULL) {

        struct libusb_device_descriptor desc;
        r = libusb_get_device_descriptor(dev, &desc);

        if (r < 0) {
            qCritical() << "failed to get device descriptor";
            return -1;
        }

        if(product_id == value.sprintf("%04x",desc.idProduct) && vendor_id == value.sprintf("%04x",desc.idVendor))
        {
            qDebug() << "Device detected";
            qDebug() << "ID VENDOR      : " << vendor_id;
            qDebug() << "ID PRODUCT     : " << product_id;

            r = libusb_open(dev,&_handle);

            if (r < 0)
            {
                qCritical() << "error opening device";
                return -1;

            } else {

                qDebug() << "device handle ready";
                *handle = _handle;
                return 0;
            }
        }
    }

    qCritical() << "Device not detected";
    return -1;
}

int UsbUtil::dev_open(libusb_device_handle *device_handle)
{
    int r;
    r =  libusb_kernel_driver_active(device_handle,0);

    if(r)
    {
        qDebug() << "Kernel driver is active";
        r = libusb_detach_kernel_driver(device_handle, 0);

        if(r != 0)
        {
            qCritical() << "Couldn't detach kernel driver";
            libusb_close(device_handle);
            return -1;
        }
    }

    qDebug() << "Kernel driver detached";

    r = libusb_claim_interface(device_handle,0);

    if(r != 0)
    {
        qCritical() << "Couldn't Claim Interface";
        libusb_close(device_handle);
        return -1;
    }

    qDebug() << "Claimed interface";

    r = libusb_clear_halt(device_handle,PORT_OUT);

    if(r != 0)
    {
        qCritical() << "Error clear";
        return -1;
    }

    qDebug() << "Out clear";

    return 0;
}

int UsbUtil::dev_write(libusb_device_handle *device_handle, QString data, int out_len)
{
    int r, transferred = 0;

    const QByteArray ba = data.toLocal8Bit();
    const char *_data = ba.constData();
    unsigned char* _udata = (unsigned char*) qstrdup(_data);

    r = libusb_bulk_transfer(device_handle, PORT_OUT, _udata, out_len, &transferred, USB_TIMEOUT);

    free(_udata);

    if(r < 0)
    {
        qCritical() << "Error on USB transfert";
        return -1;
    }

    if (transferred < 0)
    {
        qCritical() << "transferred data empty";
        return -1;
    }

    if (transferred != out_len)
    {
        qCritical() << "Error on USB transfert (data not complete)";
        return -2;
    }

    // Too verbose log
    //qDebug() << "Transfert success";
    //qDebug() << "size :" << out_len << " bytes";

    return 0;

}

int UsbUtil::dev_close(libusb_device_handle *device_handle)
{
    int r;

    r = libusb_release_interface(device_handle, 0);

    if(r != 0)
    {
        qCritical() << "Release interface error.";
        return -1;
    }

    qDebug() << "Release interface";

    r = libusb_attach_kernel_driver(device_handle,0);

    if(r != 0)
    {
        qCritical() << "Couldn't attach driver to kernel";
        return -1;
    }

    qDebug() << "Driver attached to kernel";

    libusb_close(device_handle);

    return 0;
}
