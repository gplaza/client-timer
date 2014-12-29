#-------------------------------------------------
#
# Project created by QtCreator 2013-07-09T17:36:08
#
#-------------------------------------------------

QT       += core sql network
QT       -= gui

TARGET = client-timer
target.path = /home/pi/client-timer

CONFIG   += console qtestlib
CONFIG   -= app_bundle

TEMPLATE = app
TARGET = client-timer
INSTALLS += target

DEPENDPATH += .
INCLUDEPATH += .

include(3rdparty/Add-on_display/src/display.pri)
include(3rdparty/Add-on_soap/src/soap.pri)
include(3rdparty/Add-on_mifare/src/mifare.pri)
include(3rdparty/Add-on_logger/src/logger.pri)
include(3rdparty/Add-on_buzzer/src/buzzer.pri)
include(3rdparty/Add-on_serial/src/serial.pri)
include(3rdparty/Add-on_printer/src/printer.pri)
include(3rdparty/Add-on_protector/src/protector.pri)
include(3rdparty/Add-on_configurator/src/configurator.pri)
include(3rdparty/Add-on_fingerprint/src/fingerprint.pri)

INCLUDEPATH += /mnt/rasp-pi-rootfs/usr/local/qt5pi/include/ /mnt/rasp-pi-rootfs/usr/local/include/

SOURCES += main.cpp \
    credencial.cpp \
    acceso.cpp \
    persona.cpp \
    screen.cpp \
    bdd.cpp \
    synchroniser.cpp \
    serviceaccess.cpp \
    soapclient.cpp \
    fingerprint.cpp

HEADERS += \
    credencial.h \
    acceso.h \
    persona.h \
    screen.h \
    bdd.h \
    synchroniser.h \
    serviceaccess.h \
    soapclient.h \
    fingerprint.h

#-------------
# Test Class
#-------------

SOURCES += \
    test/testserviceaccess.cpp \
    test/testfingerprint.cpp \
    test/testbdd.cpp

HEADERS += \
    test/testfingerprint.h \
    test/testserviceaccess.h \
    test/testbdd.h

for(deploymentfolder, DEPLOYMENTFOLDERS) {
    item = item$${deploymentfolder}
    itemsources = $${item}.sources
    $$itemsources = $$eval($${deploymentfolder}.source)
    itempath = $${item}.path
    $$itempath= $$eval($${deploymentfolder}.target)
    export($$itemsources)
    export($$itempath)
    DEPLOYMENT += $$item
}

installPrefix = /home/pi/$${TARGET}

for(deploymentfolder, DEPLOYMENTFOLDERS) {
    item = item$${deploymentfolder}
    itemfiles = $${item}.files
    $$itemfiles = $$eval($${deploymentfolder}.source)
    itempath = $${item}.path
    $$itempath = $${installPrefix}/$$eval($${deploymentfolder}.target)
    export($$itemfiles)
    export($$itempath)
    INSTALLS += $$item
}




