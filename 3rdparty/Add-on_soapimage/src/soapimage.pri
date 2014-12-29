INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS	+=      $$PWD/Foto_USCOREprincipalPortBinding.nsmap \
                $$PWD/soapFoto_USCOREprincipalPortBindingProxy.h \
                $$PWD/soapH.h \
                $$PWD/soapStub.h \
		$$PWD/stdsoap2.h

                #$$PWD/Ws_USCOREPrincipalPortBinding.nsmap \
                #$$PWD/soapWs_USCOREPrincipalPortBindingProxy.h \

SOURCES +=	$$PWD/soapC.cpp \
                #$$PWD/soapWs_USCOREPrincipalPortBindingProxy.cpp \
                $$PWD/soapFoto_USCOREprincipalPortBindingProxy.cpp \
		$$PWD/stdsoap2.cpp

