//
// Created by Dominic Thompson on 19/7/2023.
//

#include "config/Config.h"

using namespace std;

bool Config::loaded = false;
string Config::up_api_key;

Config::Config() {
    // static constructor
    if (loaded) return;
    auto config_ = toml::parse("/Users/dominicthompson/git/up-client/src/config/config.toml");

    // Load all values
    up_api_key = toml::find<string>(config_, "up_api_key");

    loaded = true;
}
