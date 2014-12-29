INCLUDEPATH += /mnt/rasp-pi-rootfs/usr/include/libusb-1.0/ $$PWD
DEPENDPATH += /mnt/rasp-pi-rootfs/usr/include/libusb-1.0/ $$PWD
PRE_TARGETDEPS += /mnt/rasp-pi-rootfs/usr/lib/arm-linux-gnueabihf/libusb.a

HEADERS                += $$PWD/printer.h \
                          $$PWD/usbutil.h

SOURCES                += $$PWD/printer.cpp \
                          $$PWD/usbutil.cpp

LIBS 		       += -L/mnt/rasp-pi-rootfs/usr/lib/arm-linux-gnueabihf/ -lusb-1.0 -lrt
