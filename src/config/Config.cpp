//
// Created by Dominic Thompson on 19/7/2023.
//

#include "config/Config.h"

using namespace std;

bool Config::loaded = false;
string Config::up_api_key;
int Config::pay_date;
int Config::big_amount;

Config::Config() {
    // static constructor
    if (loaded) return;
    auto config_ = toml::parse("info/config.toml");

    // Load all values
    up_api_key = toml::find<string>(config_, "up_api_key");
    pay_date = toml::find<int>(config_, "pay_date");
    big_amount = toml::find<int>(config_, "big_amount");

    loaded = true;
}
