#-------------------------------------------------
#
# Project created by QtCreator 2013-07-09T17:36:08
#
#-------------------------------------------------

QT += quick gui core sql network

TARGET = client-timer
target.path = /home/pi/client-timer

#CONFIG   += qtestlib
#CONFIG   -= app_bundle

TEMPLATE = app
TARGET = client-timer
INSTALLS += target

DEPENDPATH += .
INCLUDEPATH += .

include(addon-printer-qt/printer.pri)
include(addon-protector-qt/protector.pri)
include(addon-display-qt/display.pri)
include(addon-rfid-qt/rfid.pri)
include(addon-soap-qt/soap.pri)
include(addon-buzzer-qt/buzzer.pri)
include(addon-logger-qt/logger.pri)
include(addon-fingerprint-qt/fingerprint.pri)
include(addon-configurator-qt/configurator.pri)

INCLUDEPATH += /mnt/rpi-rootfs/opt/qt5-rpi/include/ /mnt/rpi-rootfs/usr/local/include/ /mnt/rpi-rootfs/usr/include
QML_IMPORT_PATH = qml qml/jbQuick/Charts

qml_folder.source = qml
DEPLOYMENTFOLDERS = qml_folder

SOURCES += main.cpp \
    credencial.cpp \
    acceso.cpp \
    persona.cpp \
    screen.cpp \
    bdd.cpp \
    synchroniser.cpp \
    serviceaccess.cpp \
    soapclient.cpp \
    fingerprint.cpp \
    configuratoradapter.cpp \
    fotoprovider.cpp

HEADERS += \
    credencial.h \
    acceso.h \
    persona.h \
    screen.h \
    bdd.h \
    synchroniser.h \
    serviceaccess.h \
    soapclient.h \
    fingerprint.h \
    configuratoradapter.h \
    fotoprovider.h

#-------------
# Web Service
#-------------

SOURCES += \
    web-service/casino/casinoC.cpp \
    web-service/casino/casinoClient.cpp \
    web-service/foto/fotoC.cpp \
    web-service/foto/fotoClient.cpp

HEADERS += \
    web-service/casino/casinoH.h \
    web-service/casino/casinoStub.h \
    web-service/foto/fotoH.h \
    web-service/foto/fotoStub.h

OTHER_FILES += web-service/foto/foto.nsmap \
               web-service/casino/casino.nsmap

#-------------
# Test Class
#-------------
#
#SOURCES += \
#    test/testserviceaccess.cpp \
#    test/testfingerprint.cpp \
#    test/testbdd.cpp

#HEADERS += \
#    test/testfingerprint.h \
#    test/testserviceaccess.h \
#    test/testbdd.h

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




