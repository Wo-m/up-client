
#pragma once
#include "model/Tags.h"
#include "model/Transaction.h"
#include "service/DataManager.h"
#include "service/DateHelper.h"
#include <date/date.h>
#include <fmt/core.h>
#include <iostream>
#include <sqlite_orm/sqlite_orm.h>
#include <string>

using namespace std;

vclass Menu
{
public:
    void main()
    {
        StartUp();
        string input;
        while (1)
        {
            input = GetInput(fmt::format("{}\n{}\n{}\n{}\n{}\n",
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
                    AddTransaction();
                    break;
                case 2:
                    Snapshots();
                    break;
                default:
                    fmt::print("not an option, try again\n");
            }
            input.clear();
        }
    }

private:
    DataManager data_manager_;

    string GetInput(std::string question, std::string default_choice = "")
    {
        fmt::print("{}\n", question);
        string input;
        getline(cin, input);

        if (input.empty())
            return default_choice;

        return input;
    }

    void StartUp()
    {
        data_manager_.UpdateTransactions(false);
    }

    void AddTransaction()
    {
        auto date = DateHelper::ToYearMonthDay(GetInput("date (dd/mm/yy) (empty for today)", DateHelper::ToStringDDMMYY(DateHelper::GetToday())));
        auto amount = GetInput("amount (in cents: 10000 -> $100.00)");
        auto description = GetInput("description");
        auto tag = tag_from_string(GetInput("tag (empty for none)", "NONE"));

        Transaction transaction({ 0, "", (int)(stof(amount) * 100), description, DateHelper::ConvertToRFC(date), tag, true });

        DataManager::AddTransaction(transaction);
    }

    void Snapshots()
    {
        string choice =
            GetInput(fmt::format("please pick an option:\n{}\n{}\n", "1: weekly", "2: pay cycle", "0: back"));
        string show = GetInput(fmt::format("show transactions?:\n{}\n{}\n", "0: no", "1: yes"));

        DataManager::GenerateSnapshots(stoi(choice), stoi(show));
    }
};
