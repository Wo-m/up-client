//
// Created by Dominic Thompson on 19/7/2023.
//

#pragma once

#include <toml.hpp>
#include <string>

class Config {
    static bool loaded;
public:
    static std::string up_api_key;
    static int pay_date;
    Config();
};
