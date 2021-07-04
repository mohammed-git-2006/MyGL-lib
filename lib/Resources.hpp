#pragma once

#include <string>

namespace Res {
    std::string base_path = "./res/";

    std::string get(std::string item) {
        return Res::base_path + item;
    }
}