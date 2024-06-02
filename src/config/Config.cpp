//
// Created by Dominic Thompson on 19/7/2023.
//

#include "config/Config.h"

using namespace std;

bool Config::loaded = false;
string Config::up_api_key;
int Config::pay_date;
int Config::big_amount;
string Config::begin;
int Config::start_balance;
int Config::backdated_fetch_days;

Config::Config()
{
    // static constructor
    if (loaded)
        return;
    auto config_ = toml::parse("info/config.toml");

    // Load all values
    up_api_key = toml::find<string>(config_, "up_api_key");
    pay_date = toml::find<int>(config_, "pay_date");
    big_amount = toml::find<int>(config_, "big_amount");
    begin = toml::find<string>(config_, "begin");
    start_balance = toml::find<int>(config_, "start_balance");
    backdated_fetch_days = toml::find<int>(config_, "backdated_fetch_days");

    loaded = true;
}
