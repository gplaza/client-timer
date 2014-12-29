INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS                += $$PWD/display_hd44780.h \
                          $$PWD/display_i2c.h \
                          $$PWD/idisplay.h

SOURCES                += $$PWD/display_hd44780.cpp \
                          $$PWD/display_i2c.cpp

LIBS 		       += -lwiringPiDev -lwiringPi -lpthread

