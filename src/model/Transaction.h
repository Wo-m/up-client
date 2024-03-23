//
// Created by Dominic Thompson on 18/7/2023.
//

#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <fmt/format.h>

struct Transaction {
    float amount;
    std::string description;
    std::string category;
    std::string subCategory;
    std::string createdAt;

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

    static Transaction csv_line_to_transaction(std::string line) {
        std::string amount, createdAt, category, subCategory, description;
        std::istringstream iss(line);

        std::getline(iss, amount, ',');
        std::getline(iss, createdAt, ',');
        std::getline(iss, category, ',');
        std::getline(iss, subCategory, ',');
        std::getline(iss, description);

        return {
                stof(amount),
                description,
                category,
                subCategory,
                createdAt
        };
    }
};
