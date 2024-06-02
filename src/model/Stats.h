#pragma once
#include "model/Tags.h"
#include <fmt/core.h>
#include <fmt/format.h>
#include <model/Amount.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>

struct Stats
{
    Amount income;
    Amount expense;
    Amount total;
    std::string last_date;
    std::map<Tag, Amount> tag_to_amount;

    std::string summary()
    {
        std::string tags_summary;    // expense only
        for (auto item : tag_to_amount)
        {
            if (item.first == INCOME)
                continue;
            tags_summary.append(fmt::format("{}: {:.2f} ", to_string(item.first), item.second));
        }

        return fmt::format("income: {:.2f} expense: {:.2f} total {:.2f}\n{}\n", income, expense, total, tags_summary);
    }
};
