#include "service/DataManager.h"
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

Stats DataManager::calculate_stats(std::vector<Transaction> transactions) {
    float income = 0;
    float expense = 0;
    for (auto& t : transactions) {
        if (t.category == "income") {
            income += t.amount;
        } else {
            expense += t.amount;
        }
    }

    return {
        income,
        expense,
        income + expense,
        transactions.at(transactions.size() - 1).createdAt
    };
}

void write_stats(Stats stats) {
    std::string json = nlohmann::to_string(stats.to_json());
    std::ofstream file;
    file.open("stats.json");
    file << json;
    file.close();
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
