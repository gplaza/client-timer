INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += $$PWD/buzzer.h \
           $$PWD/piezo.h

SOURCES += $$PWD/buzzer.cpp \
           $$PWD/piezo.cpp

LIBS += -lwiringPiDev -lwiringPi -lpthread

