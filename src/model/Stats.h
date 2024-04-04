#include "model/Tags.h"
#include <fmt/core.h>
#include <fmt/format.h>
#include <string>
#include <nlohmann/json_fwd.hpp>
#include <nlohmann/json.hpp>

struct Stats {
    float income;
    float expense;
    float total;
    std::string last_date;
    std::map<Tag, float> tag_to_amount;

    std::string summary() {
        std::string tags_summary = "";
        float tags_total = 0;
        for (auto item: tag_to_amount) {
            tags_summary.append(fmt::format("{}: {:.2f} ", to_string(item.first), item.second));
            tags_total += item.second;
        }
        tags_summary.append(fmt::format("\nexpense - TAGS: {:.2f}", expense - tags_total));

        return fmt::format("income: {:.2f} expense: {:.2f} total {:.2f}\n{}\n", income, expense, total, tags_summary);
    }
};
