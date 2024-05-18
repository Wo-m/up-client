//
// Created by Dominic Thompson on 18/7/2023.
//

#pragma once

#include <sstream>
#include <string>
#include <fmt/format.h>
#include <model/Tags.h>

struct Transaction {
    float amount;
    std::string description;
    std::string createdAt;
    Tag tag;

    std::string csv_entry() {
        return fmt::format("{:.2f},{},{},{}\n", 
                           amount,
                           createdAt,
                           description,
                           to_string(tag));
    }

    std::string summary() const {
        return fmt::format("amount: {:.2f} description: {} createdAt: {} tag: {}",
                           amount,
                           description,
                           createdAt,
                           to_string(tag)); 
    }

    static Transaction csv_line_to_transaction(std::string line) {
        std::string amount, createdAt, description, tag;
        std::istringstream iss(line);

        std::getline(iss, amount, ',');
        std::getline(iss, createdAt, ',');
        std::getline(iss, description, ',');
        std::getline(iss, tag, ',');

        return {
                stof(amount),
                description,
                createdAt,
                tag_from_string(tag)
        };
    }
};
