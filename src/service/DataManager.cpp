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
    file.open("info/info.json");
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
    std::ifstream ifs("info/categories.json");
    return nlohmann::json::parse(ifs);
}

void DataManager::correct_nulls(std::vector<Transaction>& transactions) {
    auto it = transactions.begin();
    while (it != transactions.end()) {
        auto& transaction = it;
        if (transaction->category != "null") {
            it++;
            continue;
        }
        auto choice = correct_nulls(*it);
        if (choice == -1)
            it = transactions.erase(it);
        it++;
    }
}

int DataManager::correct_nulls(Transaction& transaction) {

    nlohmann::json categories = getCategories();
    fmt::print("Please add category and sub-category for the following entry:\n");
    fmt::print("{}\n", transaction.summary());
    
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
        return -1;
    }
    transaction.category = std::string(categories.at(index).at(1));
    transaction.subCategory = std::string(categories.at(index).at(0));
    return 0;
}

void DataManager::add_new_transaction(Transaction& transaction) {
    auto csv_line = transaction.csv_entry();

    std::ifstream inputFile("info/data.csv");
    std::ofstream tempFile("temp.csv");

    std::string line;
    bool done = false;
    while (getline(inputFile, line)) {
        auto t = Transaction::csv_line_to_transaction(line);

        if (!done && (transaction.createdAt < t.createdAt)) {
            // Add your new line after this line
            tempFile << csv_line;
            // Add your new line here
            tempFile << line << std::endl;

            done = true;
        } else {
            tempFile << line << std::endl;
        }
    }

    inputFile.close();
    tempFile.close();

    // Replace the original file with the modified temp file
    remove("info/data.csv");
    rename("temp.csv", "info/data.csv");
}

void DataManager::write(std::vector<Transaction> transactions) {
    std::ofstream csv;
    csv.open("info/data.csv", std::ios_base::app);

    float expense = 0;
    float income = 0;

    for (auto& t : transactions) {
        fmt::print("{}\n", t.summary());
        csv << t.csv_entry();   
    }
    csv.close();

    update_info(transactions.back().createdAt);
}
