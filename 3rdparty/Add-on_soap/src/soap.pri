INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS	+=      $$PWD/Ws_USCOREPrincipalPortBinding.nsmap \
                $$PWD/soapH.h \
                $$PWD/soapStub.h \
                $$PWD/stdsoap2.h

SOURCES +=	$$PWD/soapC.cpp \
                $$PWD/stdsoap2.cpp \
                $$PWD/soapClient.cpp

