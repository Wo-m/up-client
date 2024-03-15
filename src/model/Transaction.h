//
// Created by Dominic Thompson on 18/7/2023.
//

#pragma once

#include <string>
#include <fmt/format.h>

struct Transaction {
    std::string id;
    float amount;
    std::string description;
    std::string category;
    std::string subCategory;
    std::string createdAt;
    std::string settledAt;

    std::string csv_entry() {
        return fmt::format("{:.2f},{},{},{},{}\n", 
                           amount,
                           createdAt,
                           category,
                           subCategory,
                           description);
    }

    std::string summary() const {
        return fmt::format("amount: {:.2f} description: {} category: {} subCategory: {} createdAt: {}",
                           amount,
                           description,
                           category,
                           subCategory,
                           createdAt); 
    }
};
