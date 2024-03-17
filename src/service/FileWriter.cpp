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

void write_stats(Stats stats, bool replace) {
    float curr_expense, curr_income, curr_total;
    nlohmann::json stats_json;
    if (!replace) {
        std::ifstream stream("stats.json");
        stats_json = nlohmann::json::parse(stream);

        curr_income = stof((std::string)stats_json["income"]);
        curr_expense = stof((std::string)stats_json["expense"]);
        curr_total = stof((std::string)stats_json["total"]);
    } else {
        curr_total = 0;
        curr_expense = 0;
        curr_income = 0;
    }

    stats_json["income"] = fmt::format("{:.2f}", curr_income + stats.income);
    stats_json["expense"] = fmt::format("{:.2f}", curr_expense + stats.expense);
    stats_json["total"] = fmt::format("{:.2f}", curr_total + stats.income + stats.expense);
    stats_json["last_date"] = stats.last_date;

    std::string json = nlohmann::to_string(stats_json);
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
    write_stats(calculate_stats(transactions), true);
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

    write_stats(calculate_stats(transactions), false);
}
