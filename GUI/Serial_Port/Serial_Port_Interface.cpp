#include "Serial_Port_Interface.h"

namespace RS_232{
    Serial_Port& getline(
        Serial_Port& src,
        Serial_Port::vol_str_type& dest,
        Serial_Port::byte_type delim
    ){
        dest.resize(src.read_rate());
        for(Serial_Port::size_type i(0); i < dest.size(); ++i){
            Serial_Port::byte_type hold;
            src.read(hold);
            if(hold == delim)   break;
            dest[i] = hold;
        }
        return src;
    }
}