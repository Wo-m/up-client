
#pragma once
#include "config/Config.h"
#include "model/Tags.h"
#include "model/Transaction.h"
#include "service/DataManager.h"
#include "service/DateHelper.h"
#include "service/Repository.h"
#include "service/UpService.h"
#include <date/date.h>
#include <fmt/core.h>
#include <iostream>
#include <sqlite_orm/sqlite_orm.h>
#include <string>

using namespace std;

class Menu
{
public:
    void main()
    {
        start_up();
        string input;
        while (1)
        {
            input = get_input(fmt::format("{}\n{}\n{}\n{}\n{}\n",
                       "1: add new transactions",
                       "2: snapshots",
                       "-1: AdHoc",
                       "0: quit"));

            switch (stoi(input))
            {
                case 0:
                    return;
                case -1:
                    DataManager::AdHoc();
                    break;
                case 1:
                    add_new_transaction();
                    break;
                case 2:
                    snapshot_menu();
                    break;
                default:
                    fmt::print("not an option, try again\n");
            }
            input.clear();
        }
    }

private:
    DataManager data_manager_;

    string get_input(std::string question, std::string default_choice = "")
    {
        fmt::print("{}\n", question);
        string input;
        getline(cin, input);

        if (input.empty())
            return default_choice;

        return input;
    }

    void start_up()
    {
        find_new_transactions();
    }

    void find_new_transactions()
    {
        data_manager_.UpdateTransactions(false);
    }

    void add_new_transaction()
    {
        auto date = DateHelper::ToYearMonthDay(get_input("date (dd/mm/yy) (empty for today)", DateHelper::ToStringDDMMYY(DateHelper::GetToday())));
        auto amount = get_input("amount (in cents: 10000 -> $100.00)");
        auto description = get_input("description");
        auto tag = tag_from_string(get_input("tag (empty for none)", "NONE"));

        Transaction transaction({ 0, "", (int)(stof(amount) * 100), description, DateHelper::ConvertToRFC(date), tag, true });

        DataManager::AddTransaction(transaction);
    }

    void snapshot_menu()
    {
        string choice =
            get_input(fmt::format("please pick an option:\n{}\n{}\n", "1: weekly", "2: pay cycle", "0: back"));
        string show = get_input(fmt::format("show transactions?:\n{}\n{}\n", "0: no", "1: yes"));

        DataManager::GenerateSnapshots(stoi(choice), stoi(show));
    }
};
