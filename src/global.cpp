#include "global.hpp"

int32_t getRegisterNum(const std::string& regString) {
    if(regString == "sp")
        return 14;
    else if(regString == "pc")
        return 15;
    else if(regString == "status")
        return 16;
    else if(regString == "handler")
        return 17;
    else if(regString == "cause")
        return 18;
    int32_t reg = std::stoi(regString.substr(1));
    if(reg < 0 || reg > 15) {
        std::cerr<<"Invalid register number"<<std::endl;
        return -1;
    }
    return reg;
}