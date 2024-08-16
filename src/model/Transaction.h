//
// Created by Dominic Thompson on 18/7/2023.
//

#pragma once

#include <fmt/format.h>
#include <model/Amount.h>
#include <model/Tags.h>
#include <string>

using TransactionId = uint64_t;
struct Transaction
{
    TransactionId id;
    Amount amount;
    std::string description;
    std::string createdAt;
    Tag tag;
    bool manual;

    std::string csv_entry()
    {
        return fmt::format("{},{},{},{}\n", amount.base(), createdAt, description, to_string(tag));
    }

    std::string summary() const
    {
        return fmt::format("{:<50}{:<20}{:<70}{:<40}{:<20}",
                           fmt::format("id: {}", id),
                           fmt::format("amount: {:.2f}", amount),
                           fmt::format("description: {}", description),
                           fmt::format("createdAt: {}", createdAt),
                           fmt::format("tag: {}", to_string(tag)));
    }
};
