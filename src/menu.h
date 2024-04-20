
#pragma once
#include <string>
#include <fmt/core.h>
#include <iostream>
#include "service/DateHelper.h"
#include "service/UpService.h"
#include "service/DataManager.h"
#include <date/date.h>

using namespace std;

const string BEGIN = "14/02/24";

class Menu {
public:

    void main() {
        string input;
        while (1) {
            fmt::print("{}\n{}\n{}\n",
                       "1: find new transactions",
                       "2: stats",
                       "0: quit");
            cin >> input;

            switch (stoi(input)) {
                case 0:
                    return;
                case 1:
                    find_new_transactions();
                    break;
                case 2:
                    stats_menu();
                    break;
                default:
                    fmt::print("not an option, try again\n");
            }
            input.clear();
        }
    }

private:

    UpService upService;

    string get_input(std::string question) {
        fmt::print("{}\n", question);
        string input;
        cin >> input;
        return input;
    }

    void find_new_transactions() {
        auto transactions = upService.find_new_transactions();
        if (transactions.empty()) {
            return;
        }

        DataManager::correct_nulls(transactions);
        DataManager::write(transactions);
        auto new_stats = DataManager::calculate_stats(transactions);
        auto overall = DataManager::calculate_stats(upService.find_transactions(BEGIN, false)); 

        fmt::print("\nStatistics for new transactions:\n{}\nOverall:\n{}\n",
                   new_stats.summary(),
                   overall.summary());
    }

    void stats_menu() {
        string choice = get_input(
            fmt::format("please pick an option:\n{}\n{}\n{}\n{}\n",
                    "1: all",
                    "2: week (starting mon)",
                    "3: last pay",
                    "4: specific"));

        Stats stats;
        vector<Transaction> transactions;
        string date;
        switch (stoi(choice)) {
            case 1:
                transactions = upService.find_transactions(BEGIN);
                stats = DataManager::calculate_stats(transactions);
                break;
            case 2:
                transactions = upService.find_transactions(DateHelper::get_last_monday());
                stats = DataManager::calculate_stats(transactions);
                break;
            case 3:
                transactions = upService.find_transactions(DateHelper::get_last_pay());
                stats = DataManager::calculate_stats(transactions);
                break;
            case 4:
                auto since = get_input("please enter a date (dd/mm/yy)");
                transactions = upService.find_transactions(since);
                stats = DataManager::calculate_stats(transactions);
                break;
        }

        fmt::print("{}\n", stats.summary());
    }
};
