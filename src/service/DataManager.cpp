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

void write_stats(Stats stats) {
    std::string json = nlohmann::to_string(stats.to_json());
    std::ofstream file;
    file.open("stats.json");
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

/**
* handles manual csv edits
*/
void DataManager::recalculate_stats() {
    std::ifstream csv;
    csv.open("data.csv");
    std::string line;

    auto transactions = std::vector<Transaction>();
    while(!csv.eof()) {
        getline(csv, line);
        try {
            transactions.push_back(Transaction::csv_line_to_transaction(line));
        } catch (std::exception e) {
            // eof
        }
    }
    auto stats = calculate_stats(transactions);
    write_stats(stats);
}

Stats DataManager::get_current_stats() {
    std::ifstream stream("stats.json");
    auto stats_json = nlohmann::json::parse(stream);
    return Stats::from_json(stats_json);
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

Stats DataManager::write(std::vector<Transaction> transactions) {
    std::ofstream csv;
    csv.open("data.csv", std::ios_base::app);

    float expense = 0;
    float income = 0;

    for (auto& t : transactions) {
        fmt::print("{}\n", t.summary());
        csv << t.csv_entry();   
    }
    csv.close();

    Stats current = get_current_stats();
    current.add(calculate_stats(transactions));

    write_stats(current);
    return current;
}
