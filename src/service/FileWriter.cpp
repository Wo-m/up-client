#include "service/FileWriter.h"
#include "model/Transaction.h"
#include <fstream>
#include <ios>
#include <iostream>
#include <nlohmann/json_fwd.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

FileWriter::FileWriter() {
}

void FileWriter::write_to_csv(std::vector<Transaction> transactions) {
    std::ofstream csv;
    csv.open("data.csv", std::ios_base::app);

    float expense = 0;
    float income = 0;

    for (auto& t : transactions) {
        if (t.amount < 0) {
            expense += t.amount;
        } else {
            income += t.amount;
        }

        csv << t.csv_entry();   
    }
    csv.close();

    // update stats
    std::ifstream stream("stats.json");
    auto stats = nlohmann::json::parse(stream);

    float curr_income = stof((std::string)stats["income"]);
    float curr_expense = stof((std::string)stats["expense"]);
    float curr_total = stof((std::string)stats["total"]);

    stats["last_date"] = transactions.at(transactions.size() - 1).createdAt;
    stats["income"] = fmt::format("{:.2f}", curr_income + income);
    stats["expense"] = fmt::format("{:.2f}", curr_expense + expense);
    stats["total"] = fmt::format("{:.2f}", curr_total + income + expense);

    std::string json = nlohmann::to_string(stats);
    std::ofstream file;
    file.open("stats.json");
    file << json;
    file.close();
}
