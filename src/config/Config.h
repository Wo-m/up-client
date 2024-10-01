//
// Created by Dominic Thompson on 19/7/2023.
//

#pragma once

#include <string>
#include <toml.hpp>

class Config
{
    static bool loaded;

public:
    static std::string up_api_key;
    static std::string begin;           // start of transactions
    static int backdated_fetch_days;    // amount of days to backdate new transacions fetch.
    static int pay_date;                // monthly pay date
    Config();
};
