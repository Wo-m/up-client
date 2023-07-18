//
// Created by Dominic Thompson on 18/7/2023.
//

#pragma once

#include <string>

struct Transaction {
    std::string id;
    std::string description;
    std::string category;
    float amount;
    std::string createdAt;
    std::string settledAt;
};