#ifndef USBUTIL_H
#define USBUTIL_H
#include <libusb.h>
#include <QDebug>

#define VENDOR_ID   1d90
#define PRODUCT_ID  2060
#define PORT_OUT    0x02
#define PORT_IN     0x82

#define USB_TIMEOUT	100	    // 1 secconds

class UsbUtil
{
public:
    UsbUtil();
    int dev_find(QString vendor_id, QString product_id, libusb_device_handle **handle);
    int dev_write(libusb_device_handle *device_handle, QString data, int out_len);
    int dev_open(libusb_device_handle *device_handle);
    int dev_close(libusb_device_handle *device_handle);
};

#endif // USBUTIL_H
