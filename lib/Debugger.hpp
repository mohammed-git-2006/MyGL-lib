#pragma once

#include <iostream>

namespace Debugger
{
    void Log(std::string from, std::string content) {
        std::cout << "[LOG] @[" << from << "] :\t " << content << std::endl;
    }

    void Error(std::string from, std::string content)  {
        std::cout << "[ERROR] @[" << from << "] : " << content << std::endl;
    }
}
