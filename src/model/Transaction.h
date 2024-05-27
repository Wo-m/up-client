//
// Created by Dominic Thompson on 18/7/2023.
//

#pragma once

#include <sstream>
#include <string>
#include <fmt/format.h>
#include <model/Tags.h>
#include <model/Amount.h>

struct Transaction {
    // TODO: create new type so i don't have to wrap everything in to_dollars
    Amount amount;
    std::string description;
    std::string createdAt;
    Tag tag;

    std::string csv_entry() {
        return fmt::format("{},{},{},{}\n", 
                           amount.base(),
                           createdAt,
                           description,
                           to_string(tag));
    }

    std::string summary() const {
        return fmt::format("{:<20}{:<70}{:<40}{:<20}",
                           fmt::format("amount: {:.2f}", amount),
                           fmt::format("description: {}", description),
                           fmt::format("createdAt: {}", createdAt),
                           fmt::format("tag: {}", to_string(tag)));
    }

    static Transaction csv_line_to_transaction(std::string line) {
        std::string amount, createdAt, description, tag;
        std::istringstream iss(line);

        std::getline(iss, amount, ',');
        std::getline(iss, createdAt, ',');
        std::getline(iss, description, ',');
        std::getline(iss, tag, ',');

        return {
                stoi(amount),
                description,
                createdAt,
                tag_from_string(tag)
        };
    }
};

