#-------------------------------------------------
#
# Project created by QtCreator 2013-07-09T17:36:08
#
#-------------------------------------------------

QT += core sql network

TARGET = client-timer
target.path = /home/pi/client-timer

TEMPLATE = app
TARGET = client-timer
INSTALLS += target

DEPENDPATH += .
INCLUDEPATH += .

include(addon-door-qt/door.pri)
include(addon-protector-qt/protector.pri)
include(addon-rfid-qt/rfid.pri)
include(addon-buzzer-qt/buzzer.pri)
include(addon-logger-qt/logger.pri)
include(addon-fingerprint-qt/fingerprint.pri)
include(addon-configurator-qt/configurator.pri)

INCLUDEPATH += /mnt/rpi-rootfs/opt/qt5-rpi/include/ /mnt/rpi-rootfs/usr/local/include/ /mnt/rpi-rootfs/usr/include
LIBS += -lsqlite3

SOURCES += main.cpp \
    credencial.cpp \
    acceso.cpp \
    persona.cpp \
    bdd.cpp \
    synchroniser.cpp \
    serviceaccess.cpp \
    fingerprint.cpp \
    configuratoradapter.cpp

HEADERS += \
    credencial.h \
    acceso.h \
    persona.h \
    bdd.h \
    synchroniser.h \
    serviceaccess.h \
    fingerprint.h \
    configuratoradapter.h

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



