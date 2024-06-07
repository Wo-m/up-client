//
// Created by Dominic Thompson on 19/7/2023.
//

#include "config/Config.h"

using namespace std;

bool Config::loaded = false;
string Config::up_api_key;
string Config::begin;
int Config::start_balance;
int Config::backdated_fetch_days;

int Config::pay_date;
int Config::pay_amount;

int Config::rent_cycle;
int Config::rent_amount;

int Config::big_amount;

Config::Config()
{
    // static constructor
    if (loaded)
        return;
    auto config_ = toml::parse("info/config.toml");

    // Load all values
    up_api_key = toml::find<string>(config_, "up_api_key");
    begin = toml::find<string>(config_, "begin");
    start_balance = toml::find<int>(config_, "start_balance");
    backdated_fetch_days = toml::find<int>(config_, "backdated_fetch_days");

    pay_date = toml::find<int>(config_, "pay_date");
    pay_amount = toml::find<int>(config_, "pay_amount");

    rent_cycle = toml::find<int>(config_, "rent_cycle");
    rent_amount = toml::find<int>(config_, "rent_amount");

    big_amount = toml::find<int>(config_, "big_amount");

    loaded = true;
}
