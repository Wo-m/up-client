#include "config/Config.h"
#include "model/Transaction.h"
#include "service/UpService.h"
#include "service/FileWriter.h"
#include <cstdio>
#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json_fwd.hpp>
#include <ostream>
#include <string>
#include <vector>

using namespace std;

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

std::string getLastTransactionDate() {
    ifstream last_date;
    last_date.open("last_date.txt");
    string line = "";
    getline(last_date, line);
    return line;
}

int main() {
    // This is just for testing in its current state
    Config();

    UpService upService;
    Account account = upService.getTransactionalAccount();
    string last_date = getLastTransactionDate();
    auto transactions = upService.getTransactions(account.id, last_date);
    correctNulls(transactions);
    FileWriter::write_to_csv(transactions);
}
