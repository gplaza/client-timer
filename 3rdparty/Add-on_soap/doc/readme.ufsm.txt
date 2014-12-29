wsdl2h -s -o controlAsistancia.h http://ssodes.usm.cl/Ws_Asistencia_ControlAcceso/Ws_Principal?wsdl

Editer fichier controlAsistancia.h :

class ns1__Modulo_USCOREAsistencia
{ public:
    std::string*                         rut                            0;	///< Optional element.
    std::string*                         numerotarjeta                  0;	///< Optional element.
    time_t*                              fecha                          0;	///< Optional element.
    std::string*                         numerodispositivo              0;	///< Optional element.
    int                                  tipomarca                      1;	///< Required element.
    xsd__base64Binary*                   foto                           0;	///< Nillable pointer.
    struct soap                         *soap                          ;
};

class ns1__Modulo_USCOREAsistenciaResponse
{ public:
    std::wstring*                         return_                        0;
    struct soap                         *soap                          ;
};

soapcpp2 -I"/usr/local/share/gsoap/import" -1 controlAsistancia.h

rm Ws_USCOREPrincipalPortBinding.Modulo_Asistencia.re*
rm soapServer*
rm soapClientLib.cpp
rm controlAsistancia.h

