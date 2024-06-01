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
    static int pay_date;                // monthly pay date
    static int big_amount;              // amount to be classified as BIG transaction
    static std::string begin;           // start of transactions
    static int start_balance;         // balance before use
    static int backdated_fetch_days;    // amount of days to backdate new transacions fetch. Used to removed pending transactions
    Config();
};
