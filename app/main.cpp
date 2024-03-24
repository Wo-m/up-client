#include "config/Config.h"
#include "model/Transaction.h"
#include "service/UpService.h"
#include "service/DataManager.h"
#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <vector>

using namespace std;

// TODO: move to Menu class?
// Init
Config config;
UpService upService;

nlohmann::json getCategories() {
    std::ifstream ifs("categories.json");
    return nlohmann::json::parse(ifs);
}

void correctNulls(vector<Transaction>& transactions) {
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

        string choice;
        cin >> choice;
        int index = stoi(choice);
        if (index == -1) {
            it = transactions.erase(it);
            continue;
        }
        transaction->category = string(categories.at(index).at(1));
        transaction->subCategory = string(categories.at(index).at(0));
        it++;
    }
}

void update_transactions() {
    auto transactions = upService.find_new_transactions();
    if (transactions.empty())
        return;

    correctNulls(transactions);
    Stats current = DataManager::write(transactions);
    auto new_stats = DataManager::calculate_stats(transactions);

    fmt::print("Statistics for new transactions:\n{}\n Overall:\n{}\n",
               new_stats.summary(),
               current.summary());
}

void stats() {
    fmt::print("please enter a date (dd/mm/yy) [0 for all]: ");
    string since;
    cin >> since;
    

    Stats stats;
    if (since == "0") {
        stats = DataManager::get_current_stats();
    } else {
        auto transactions = upService.find_transactions(since);
        stats = DataManager::calculate_stats(transactions);
    }

    fmt::print("{}\n", stats.summary());
}

int main() {
    Config();

    // Menu
    // update_transactions();
    string input;
    while (1) {
        fmt::print("{}\n{}\n{}\n{}\n",
                   "1: find new transactions",
                   "2: stats",
                   "3: recalc stats",
                   "0: quit");
        cin >> input;

        switch (stoi(input)) {
            case 0:
                return 0;
            case 1:
                update_transactions();
                break;
            case 2:
                stats();
                break;
            case 3:
                DataManager::recalculate_stats();
                break;
            default:
                fmt::print("not an option, try again\n");
        }
        input.clear();
    }
}
