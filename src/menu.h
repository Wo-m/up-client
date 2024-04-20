
#pragma once
#include <string>
#include <fmt/core.h>
#include <iostream>
#include "model/Tags.h"
#include "model/Transaction.h"
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
            fmt::print("{}\n{}\n{}\n{}\n{}\n",
                       "1: find new transactions",
                       "2: stats",
                       "3: add new transactions",
                       "4: snapshots",
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
                case 3:
                    add_new_transaction();
                    break;
                case 4:
                    snapshot_menu();
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
        auto date = get_input("date (dd/mm/yy)");
        auto amount = get_input("amount");
        auto description = get_input("description");
        auto tag = tag_from_string(get_input("tag"));

        Transaction transaction({
            stof(amount),
            description,
            "null",
            "null",
            DateHelper::convertToRFC3339(date),
            tag
        });

        DataManager::correct_nulls(transaction);
        DataManager::add_new_transaction(transaction);
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

        string date;
        switch (stoi(choice)) {
            case 1:
                date = BEGIN;
                break;
            case 2:
                date = date::format("%y/%m/%d", DateHelper::get_last_monday());
                break;
            case 3:
                date = date::format("%y/%m/%d", DateHelper::get_last_pay());
                break;
            case 4:
                date = get_input("please enter a date (yy/mm/dd)");
                break;
        }
        auto transactions = upService.find_transactions(date);
        auto stats = DataManager::calculate_stats(transactions);

        fmt::print("{}\n", stats.summary());
    }
};
