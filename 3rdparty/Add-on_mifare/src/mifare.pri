INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS                += $$PWD/rfid.h \
                          $$PWD/rfid-utils.h

SOURCES                += $$PWD/rfid.cpp \
                          $$PWD/rfid-utils.cpp

LIBS 		       += -lnfc -lfreefare
