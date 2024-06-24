
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
        // do this on boot
        start_up();
        string input;
        while (1)
        {
            fmt::print("{}\n{}\n{}\n{}\n{}\n{}\n",
                       "1: add new transactions",
                       "2: stats",
                       "3: snapshots",
                       "4: savings",
                       "-1: AdHoc",
                       "0: quit");
            cin >> input;

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

    string get_input(std::string question)
    {
        fmt::print("{}\n", question);
        string input;
        cin >> input;
        return input;
    }

    void add_new_transaction()
    {
        auto date = DateHelper::ToYearMonthDay(get_input("date (dd/mm/yy)"));
        auto amount = get_input("amount");
        auto description = get_input("description");
        auto tag = tag_from_string(get_input("tag"));

        Transaction transaction({ (int)(stof(amount) * 100), description, DateHelper::ConvertToRFC(date), tag, true });

        DataManager::AddTransaction(transaction);
    }

    void savings()
    {
        auto accounts = upService.GetAccounts();
        DataManager::CalculateSaved(accounts);
    }

    void find_new_transactions()
    {
        auto transactions = upService.FindNewTransactions();
        if (transactions.empty())
        {
            fmt::print("no new transactions\n");
            return;
        }

        DataManager::UpdateTransactions(transactions);
    }

    void snapshot_menu()
    {
        string choice =
            get_input(fmt::format("please pick an option:\n{}\n{}\n", "1: weekly", "2: pay cycle", "0: back"));
        string show = get_input(fmt::format("show transactions?:\n{}\n{}\n", "0: no", "1: yes"));

        DataManager::GenerateSnapshots(stoi(choice), stoi(show));
    }

    void stats_menu()
    {
        string choice = get_input(fmt::format("please pick an option:\n{}\n{}\n{}\n{}\n",
                                              "1: all",
                                              "2: week (starting mon)",
                                              "3: last pay",
                                              "4: specific"));

        date::year_month_day date;
        switch (stoi(choice))
        {
            case 1:
                date = DateHelper::ToYearMonthDay(Config::begin);
                break;
            case 2:
                date = DateHelper::GetLastMonday();
                break;
            case 3:
                date = DateHelper::GetLastPay();
                break;
            case 4:
                date = DateHelper::ToYearMonthDay(get_input("please enter a date (dd/mm/yy)"));
                break;
        }

        auto today = DateHelper::GetToday();
        auto transactions = DataManager::FindTransactions(date, today, true);
        auto stats = DataManager::CalculateStats(transactions);

        fmt::print("{}\n", stats.summary());
    }

    void start_up()
    {
        find_new_transactions();

        using namespace sqlite_orm;
        auto storage = Repository::get_storage();
        auto today = date::sys_days(DateHelper::GetToday());

        // auto add pay
        auto income_transactions =
            storage.get_all<Transaction>(where(c(&Transaction::tag) == INCOME), order_by(&Transaction::createdAt));
        auto last_pay_date = DateHelper::RFCToYearMonthDay(income_transactions.back().createdAt);

        if (last_pay_date + date::months(1) <= today)
        {
            fmt::print("auto adding pay:\n");
            auto new_pay_date = last_pay_date + date::months(1);
            assert(new_pay_date.day() == date::day(Config::pay_date));
            while (new_pay_date <= today) {
                Transaction t{
                    Config::pay_amount, "vivcourt", DateHelper::ConvertToRFC(new_pay_date), INCOME, true
                };
                fmt::print("{}\n", t.summary());
                storage.replace(t);
                new_pay_date += date::months(1);
            }
        }

        // auto add rent
        auto rent_transactions = storage.get_all<Transaction>(where(c(&Transaction::description) == "rent"),
                                                              order_by(&Transaction::createdAt));
        auto last_rent_date = date::sys_days(DateHelper::RFCToYearMonthDay(rent_transactions.back().createdAt));

        if ((last_rent_date + date::days(Config::rent_cycle)) <= today)
        {
            fmt::print("auto adding rent:\n");
            auto new_rent_date = last_rent_date + date::days(Config::rent_cycle);
            while (new_rent_date <= today)
            {
                Transaction t{
                    Config::rent_amount, "rent", DateHelper::ConvertToRFC(date::year_month_day{ new_rent_date }), EXPECTED, true
                };

                fmt::print("{}\n", t.summary());
                storage.replace(t);
                new_rent_date += date::days(Config::rent_cycle);
            }
        }
    }
};
