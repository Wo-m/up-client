#include "service/DataManager.h"
#include "model/Tags.h"
#include "model/Transaction.h"
#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <nlohmann/json_fwd.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>


DataManager::DataManager() {
}

void update_info(std::string date) {
    auto json = nlohmann::json();
    json["last_date"] = date;
    std::ofstream file;
    file.open("info.json");
    file << json;
    file.close();

}

Stats DataManager::calculate_stats(std::vector<Transaction> transactions) {
    float income = 0;
    float expense = 0;
    auto tag_to_amount = std::map<Tag, float>();
    for (auto& t : transactions) {
        if (t.category == "income") {
            income += t.amount;
        } else {
            expense += t.amount;
        }
        auto tag = t.tag;
        if (tag == NONE) continue;

        tag_to_amount[tag] += t.amount;
    }

    return {
        income,
        expense,
        income + expense,
        transactions.at(transactions.size() - 1).createdAt,
        tag_to_amount
    };
}


nlohmann::json getCategories() {
    std::ifstream ifs("categories.json");
    return nlohmann::json::parse(ifs);
}

void DataManager::correct_nulls(std::vector<Transaction>& transactions) {
    nlohmann::json categories = getCategories();
    auto it = transactions.begin();
    while (it != transactions.end()) {
        auto& transaction = it;
        if (transaction->category != "null") {
            it++;
            continue;
        }

        fmt::print("Please add category and sub-category for the following entry:\n");
        fmt::print("{}\n", transaction->summary());
        
        // TODO: store this
        for (int i = 0; i < categories.size(); i++) {
            std::string cat = categories.at(i).at(1);
            std::string sub_cat = categories.at(i).at(0);
            fmt::print("{}: {} - {}\n", i, sub_cat, cat);
        }
        fmt::print("-1: delete\n");

        std::string choice;
        std::cin >> choice;
        int index = stoi(choice);
        if (index == -1) {
            it = transactions.erase(it);
            continue;
        }
        transaction->category = std::string(categories.at(index).at(1));
        transaction->subCategory = std::string(categories.at(index).at(0));
        it++;
    }
}

void DataManager::write(std::vector<Transaction> transactions) {
    std::ofstream csv;
    csv.open("data.csv", std::ios_base::app);

    float expense = 0;
    float income = 0;

    for (auto& t : transactions) {
        fmt::print("{}\n", t.summary());
        csv << t.csv_entry();   
    }
    csv.close();

    update_info(transactions.back().createdAt);
}
