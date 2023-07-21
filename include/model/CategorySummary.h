//
// Created by Dominic Thompson on 21/7/2023.
//

#pragma once

#include <string>
#include <map>

/**
 * CategorySummary, contains total amount
 * and subcategory amounts
 */
struct CategorySummary {
    std::string id;
    float amount;
    std::map<std::string, float> subAmounts;
};
