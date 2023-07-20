//
// Created by Dominic Thompson on 18/7/2023.
//

#pragma once

#include <string>

struct Transaction {
    std::string id;
    float amount;
    std::string description;
    std::string category;
    std::string createdAt;
    std::string settledAt;

    std::string summary() const {
        return "amount: " + std::to_string(amount)
        + " description: " + description
        + " category: " + category
        + " createdAt: " + createdAt;
    }
};