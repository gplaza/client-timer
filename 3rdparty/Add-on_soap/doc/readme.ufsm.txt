wsdl2h -o xxxxx.h http://xxx?wsdl -qxxxxx -nxxxxx

Editer fichier controlAsistancia.h :

class ns1__Modulo_USCOREAsistenciaResponse
{ public:
    std::wstring*                         return_                        0;
    struct soap                         *soap                          ;
};

soapcpp2 -I"/usr/local/share/gsoap/import" -1 xxxxx.h -L -C -x -pxxxxx -qxxxxx -n



