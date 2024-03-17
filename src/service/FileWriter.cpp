#include "service/FileWriter.h"
#include "model/Transaction.h"
#include <cstdio>
#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <nlohmann/json_fwd.hpp>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <vector>

struct Stats {
    float income;
    float expense;
    float total;
    std::string last_date;

    nlohmann::json to_json() {
        auto json = nlohmann::json::object();
        json["income"] = fmt::format("{:.2f}", income);
        json["expense"] = fmt::format("{:.2f}", expense);
        json["total"] = fmt::format("{:.2f}", total);
        json["last_date"] = last_date;

        return json;
    }

    static Stats from_json(nlohmann::json json) {
        return {
            stof((std::string)json["income"]),
            stof((std::string)json["expense"]),
            stof((std::string)json["total"]),
            json["last_date"]
        };
    }

    void add(Stats stats) {
        income += stats.income;
        expense += stats.expense;
        total = income + expense;
        last_date = stats.last_date;
    }
};

FileWriter::FileWriter() {
}

Stats calculate_stats(std::vector<Transaction> transactions) {
    float income = 0;
    float expense = 0;
    for (auto& t : transactions) {
        if (t.category == "income") {
            income += t.amount;
            fmt::print("{}\n", t.summary());
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
void FileWriter::recalculate_stats() {
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


void FileWriter::write_to_csv(std::vector<Transaction> transactions) {
    std::ofstream csv;
    csv.open("data.csv", std::ios_base::app);

    float expense = 0;
    float income = 0;

    for (auto& t : transactions) {
        csv << t.csv_entry();   
    }
    csv.close();

    std::ifstream stream("stats.json");
    auto stats_json = nlohmann::json::parse(stream);
    Stats current = Stats::from_json(stats_json);
    current.add(calculate_stats(transactions));

    write_stats(current);
}
