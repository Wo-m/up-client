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
    static int start_balance;           // balance before use
    static int backdated_fetch_days;    // amount of days to backdate new transacions fetch.

    static int pay_date;                // monthly pay date
    static int pay_amount;              // pay amount

    static int rent_cycle;              // rent cycle (i.e 7/14/28 days)
    static int rent_amount;             // rent amount

    static int big_amount;              // amount to be classified as BIG transaction
    Config();
};
