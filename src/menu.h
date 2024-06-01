
#pragma once
#include <string>
#include <fmt/core.h>
#include <iostream>
#include "config/Config.h"
#include "model/Tags.h"
#include "model/Transaction.h"
#include "service/DateHelper.h"
#include "service/UpService.h"
#include "service/DataManager.h"
#include <date/date.h>

using namespace std;

class Menu {
public:
    void main() {
        // do this on boot
        find_new_transactions();
        string input;
        while (1) {
            fmt::print("{}\n{}\n{}\n{}\n{}\n{}\n",
                       "1: add new transactions",
                       "2: stats",
                       "3: snapshots",
                       "4: savings",
                       "-1: AdHoc",
                       "0: quit");
            cin >> input;

            switch (stoi(input)) {
                case 0:
                    return;
                case -1:
                    DataManager::AdHoc();
                    break;
                case 1:
                    add_new_transaction();
                    break;
                case 2:
                    stats_menu();
                    break;
                case 3:
                    snapshot_menu();
                    break;
                case 4:
                    savings();
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

    void add_new_transaction() {
        auto date = DateHelper::to_year_month_day(get_input("date (dd/mm/yy)"));
        auto amount = get_input("amount");
        auto description = get_input("description");
        auto tag = tag_from_string(get_input("tag"));

        Transaction transaction({
            (int) stof(amount) * 100,
            description,
            DateHelper::convertToRFC3339(date),
            tag,
            true
        });

        DataManager::add_new_transaction(transaction);
    }

    void savings() {
        auto accounts = upService.get_accounts();
        DataManager::calculate_saved(accounts);
    }

    void find_new_transactions() {
        auto transactions = upService.find_new_transactions();
        if (transactions.empty()) {
            return;
        }

        DataManager::write(transactions);
        auto new_stats = DataManager::calculate_stats(transactions);
        auto today = DateHelper::get_today();
        auto overall = DataManager::calculate_stats(DataManager::find_transactions(DateHelper::to_year_month_day(Config::begin), today, false));

        fmt::print("\nStatistics for new transactions:\n{}\nOverall:\n{}\n",
                   new_stats.summary(),
                   overall.summary());
    }

    void snapshot_menu() {
        string choice = get_input(
            fmt::format("please pick an option:\n{}\n{}\n",
                    "1: weekly",
                    "2: pay cycle",
                    "0: back"));
        string show = get_input(
            fmt::format("show transactions?:\n{}\n{}\n",
                    "0: no",
                    "1: yes"));

        DataManager::snapshot(stoi(choice), stoi(show));
    }

    void stats_menu() {
        string choice = get_input(
            fmt::format("please pick an option:\n{}\n{}\n{}\n{}\n",
                    "1: all",
                    "2: week (starting mon)",
                    "3: last pay",
                    "4: specific"));

        date::year_month_day date;
        switch (stoi(choice)) {
            case 1:
                date = DateHelper::to_year_month_day(Config::begin);
                break;
            case 2:
                date = DateHelper::get_last_monday();
                break;
            case 3:
                date = DateHelper::get_last_pay();
                break;
            case 4:
                date = DateHelper::to_year_month_day(get_input("please enter a date (dd/mm/yy)"));
                break;
        }

        auto today = DateHelper::get_today();
        auto transactions = DataManager::find_transactions(date, today, true);
        auto stats = DataManager::calculate_stats(transactions);

        fmt::print("{}\n", stats.summary());
    }
};
