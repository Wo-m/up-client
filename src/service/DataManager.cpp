#include "service/DataManager.h"
#include "config/Config.h"
#include "model/Tags.h"
#include "model/Transaction.h"
#include "service/DateHelper.h"
#include <date/date.h>
#include <exception>
#include <fmt/core.h>
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
    auto tag_to_amount = std::map<Tag, float>();
    for (auto& t : transactions) {
        tag_to_amount[t.tag] += t.amount;
    }

    float income = tag_to_amount[INCOME];
    float expense = 0;

    for (auto& pair : tag_to_amount){
        if (pair.first == INCOME) continue;

        expense += pair.second;
    }

    return {
        income,
        expense,
        income + expense,
        transactions.at(transactions.size() - 1).createdAt,
        tag_to_amount
    };
}

void DataManager::calculate_saved(std::vector<Account> accounts) {
    float up = 0;
    float anz = Config::start_balance;

    for (auto& a : accounts) {
        up += a.balance;
    }

    auto stats = calculate_stats(find_transactions(Config::begin, DateHelper::yy_mm_dd(DateHelper::get_today()), false));

    anz += stats.income + stats.expense - up;

    fmt::print("anz/savings {:.2f} | up {:.2f} | income {:.2f} | expense {:.2f}\n\n", anz, up, stats.income, stats.expense);
}

void DataManager::snapshot(int choice, bool show_transactions) {
    std::vector<date::year_month_day> dates;
    switch (choice) {
        case 1:
            {
                auto last_mon = DateHelper::get_last_monday();
                auto last_mon_s = date::format("%y/%m/%d", last_mon);
                while (last_mon_s >= Config::begin) {
                    dates.insert(dates.begin(), last_mon);
                    last_mon = date::year_month_day{date::sys_days(last_mon) - date::weeks(1)};
                    last_mon_s = date::format("%y/%m/%d", last_mon);
                }
                break;
            }
        case 2:
            {
                auto last_pay = DateHelper::get_last_pay();
                auto last_pay_s = date::format("%y/%m/%d", last_pay);
                while (last_pay_s >= Config::begin) {
                    dates.insert(dates.begin(), last_pay);
                    last_pay = last_pay - date::months(1);
                    last_pay_s = date::format("%y/%m/%d", last_pay);
                }
            }
    }

    std::vector<Transaction> transactions;
    Stats stats;
    auto today =  date::format("%y/%m/%d", date::sys_days{DateHelper::get_today()});
    for (int i = 1; i <= dates.size(); i++) {
        auto since = DateHelper::yy_mm_dd(dates.at(i-1));
        if (i == dates.size()) {
            transactions = find_transactions(since, today, show_transactions);
        }
        else {
            auto to = DateHelper::yy_mm_dd(date::year_month_day{ date::sys_days(dates.at(i)) - date::days(1) });
            transactions = find_transactions(since, to, show_transactions);
        }
        if (transactions.size() == 0) continue;
        stats = calculate_stats(transactions);
        fmt::print("----- start: {} -------\n{}-----------------------------\n", since, stats.summary());
    }


}


nlohmann::json getCategories() {
    std::ifstream ifs("info/categories.json");
    return nlohmann::json::parse(ifs);
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

std::vector<Transaction> DataManager::find_transactions(const std::string &since, const std::string &to, bool print) {
    auto since_rfc = DateHelper::convertToRFC3339(since);
    auto to_rfc = DateHelper::convertToRFC3339(to, true);

    std::ifstream csv;
    csv.open("info/data.csv");
    std::string line;

    auto transactions = std::vector<Transaction>();
    Transaction t;
    while(1) {
        getline(csv, line);
        try {
            t = Transaction::csv_line_to_transaction(line);
        } catch (exception e) {
            break;
        }

        if (t.createdAt < since_rfc || t.createdAt > to_rfc) continue;
        if (print) fmt::print("{}\n", t.summary());
        transactions.push_back(t);
    }
    return transactions;
}

void DataManager::AdHoc() {
    std::ifstream inputFile("info/data.csv");
    std::ofstream tempFile("temp.csv");

    std::string line;
    bool done = false;
    while (getline(inputFile, line)) {
        auto t = Transaction::csv_line_to_transaction(line);
        tempFile << t.csv_entry();
    }

    inputFile.close();
    tempFile.close();

    // Replace the original file with the modified temp file
    remove("info/data.csv");
    rename("temp.csv", "info/data.csv");

}
