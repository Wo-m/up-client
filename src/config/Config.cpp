//
// Created by Dominic Thompson on 19/7/2023.
//

#include "config/Config.h"

using namespace std;

bool Config::loaded = false;
string Config::up_api_key;
string Config::begin;
int Config::backdated_fetch_days;
int Config::pay_date;

Config::Config()
{
    // static constructor
    if (loaded)
        return;
    auto config_ = toml::parse("info/config.toml");

    // Load all values
    up_api_key = toml::find<string>(config_, "up_api_key");
    begin = toml::find<string>(config_, "begin");
    backdated_fetch_days = toml::find<int>(config_, "backdated_fetch_days");
    pay_date = toml::find<int>(config_, "pay_date");

    loaded = true;
}
