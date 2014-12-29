INCLUDEPATH += $$PWD

unix {
    CONFIG += link_pkgconfig

    packagesExist(libudev) {
        DEFINES += HAVE_LIBUDEV
        PKGCONFIG += libudev
    }
}

PUBLIC_HEADERS += \
    $$PWD/qserialportglobal.h \
    $$PWD/qserialport.h \
    $$PWD/qserialportinfo.h

PRIVATE_HEADERS += \
    $$PWD/qserialport_p.h \
    $$PWD/qserialportinfo_p.h \
    $$PWD/qringbuffer_p.h

SOURCES += \
    $$PWD/qserialport.cpp \
    $$PWD/qserialportinfo.cpp

win32 {
    PRIVATE_HEADERS += \
        $$PWD/qserialport_win_p.h

    SOURCES += \
        $$PWD/qserialport_win.cpp \
        $$PWD/qserialportinfo_win.cpp

    !wince*: {
        LIBS += -lsetupapi -ladvapi32
    } else {
        SOURCES += \
            $$PWD/qserialport_wince.cpp \
            $$PWD/qserialportinfo_wince.cpp
    }
}

unix:!symbian {
    PRIVATE_HEADERS += \
        $$PWD/qttylocker_unix_p.h \
        $$PWD/qserialport_unix_p.h

    SOURCES += \
        $$PWD/qttylocker_unix.cpp \
        $$PWD/qserialport_unix.cpp \
        $$PWD/qserialportinfo_unix.cpp

    macx {
        SOURCES += $$PWD/qserialportinfo_mac.cpp

        LIBS += -framework IOKit -framework CoreFoundation
    }
}

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS
