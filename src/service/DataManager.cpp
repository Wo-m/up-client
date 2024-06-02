#include "service/DataManager.h"
#include "config/Config.h"
#include "model/Tags.h"
#include "model/Transaction.h"
#include "service/DateHelper.h"
#include "service/Repository.h"
#include <date/date.h>
#include <exception>
#include <fmt/core.h>
#include <fstream>
#include <ios>
#include <iostream>
#include <nlohmann/json_fwd.hpp>
#include <nlohmann/json.hpp>
#include <sqlite_orm/sqlite_orm.h>
#include <string>
#include <unordered_set>
#include <vector>
#include <sqlite3.h>


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

Stats DataManager::CalculateStats(std::vector<Transaction> transactions) {
    auto tag_to_amount = std::map<Tag, Amount>();
    for (auto& t : transactions) {
        tag_to_amount[t.tag] += t.amount;
    }

    Amount income = tag_to_amount[INCOME];
    Amount expense = 0;

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

void DataManager::CalculateSaved(std::vector<Account> accounts) {
    Amount up = 0;
    Amount anz = Config::start_balance;

    for (auto& a : accounts) {
        up += (int) (a.balance * 100);
    }

    auto stats = CalculateStats(FindTransactions(DateHelper::ToYearMonthDay(Config::begin), DateHelper::GetToday(), false));

    anz += stats.income + stats.expense - up;

    fmt::print("anz/savings {:.2f} | up {:.2f} | income {:.2f} | expense {:.2f}\n\n", anz, up, stats.income, stats.expense);
}

void DataManager::GenerateSnapshots(int choice, bool show_transactions) {
    std::vector<date::year_month_day> dates;
    switch (choice) {
        case 1:
            {
                auto last_mon = DateHelper::GetLastMonday();
                while (last_mon >= DateHelper::ToYearMonthDay(Config::begin)) {
                    dates.insert(dates.begin(), last_mon);
                    last_mon = date::year_month_day{date::sys_days(last_mon) - date::weeks(1)};
                }
                break;
            }
        case 2:
            {
                auto last_pay = DateHelper::GetLastPay();
                while (last_pay >= DateHelper::ToYearMonthDay(Config::begin)) {
                    dates.insert(dates.begin(), last_pay);
                    last_pay = last_pay - date::months(1);
                }
            }
    }

    std::vector<Transaction> transactions;
    Stats stats;
    auto today =  DateHelper::GetToday();
    for (int i = 1; i <= dates.size(); i++) {
        auto since = dates.at(i-1);
        if (i == dates.size()) {
            transactions = FindTransactions(since, today, show_transactions);
        }
        else {
            auto to = date::year_month_day{ date::sys_days(dates.at(i)) - date::days(1) };
            transactions = FindTransactions(since, to, show_transactions);
        }
        if (transactions.size() == 0) continue;
        stats = CalculateStats(transactions);
        fmt::print("----- start: {} -------\n{}-----------------------------\n", DateHelper::ToStringDDMMYY(since), stats.summary());
    }


}


nlohmann::json getCategories() {
    std::ifstream ifs("info/categories.json");
    return nlohmann::json::parse(ifs);
}

void DataManager::AddTransaction(Transaction& transaction) {
    auto storage = Repository::get_storage();
    storage.replace(transaction);
}

void DataManager::UpdateTransactions(std::vector<Transaction> transactions) {
    auto storage = Repository::get_storage();

    // get all transactions that overlap with fetch, not including manual entries
    auto db_transactions = FindTransactions(DateHelper::GetBackdate(), DateHelper::GetToday(), false, false);

    // put transactions into ascending (oldest first)
    std::reverse(transactions.begin(), transactions.end());

    // add all values to db (replaces existing)
    // and create set of dates
    std::unordered_set<std::string> dates;
    for (auto& t : transactions) {
        storage.replace(t);
        dates.emplace(t.createdAt);
    }

    // remove any values that aren't in set (usually card checks)
    for (auto & t : db_transactions) {
        if (dates.find(t.createdAt) == dates.end()) {
            storage.remove<Transaction>(t.createdAt);
            fmt::print("removing transaction: {}\n", t.summary());
        }
    }

    // we just use this to print
    auto to_rfc = DateHelper::ConvertToRFC(DateHelper::GetToday(), true);
    auto new_transactions = FindTransactions(db_transactions.back().createdAt, to_rfc, false, true);

    if (new_transactions.size() > 1) {
        new_transactions.erase(new_transactions.begin()); // last known transaction
        fmt::print("NEW TRANSACTIONS\n");
        for (auto& t : new_transactions)
             fmt::print("{}\n", t.summary());
    }

    update_info(transactions.back().createdAt);
}

std::vector<Transaction> DataManager::FindTransactions(const date::year_month_day& since, const date::year_month_day& to, bool print, bool include_manual) {
    auto since_rfc = DateHelper::ConvertToRFC(since);
    auto to_rfc = DateHelper::ConvertToRFC(to, true);
    return FindTransactions(since_rfc, to_rfc, print, include_manual);
}


std::vector<Transaction> DataManager::FindTransactions(const std::string& since_rfc, const std::string& to_rfc, bool print, bool include_manual) {
    auto storage = Repository::get_storage();

    using namespace sqlite_orm;

    // doesn't appear to be a way to prepare a statement and add to it without using strings
    // this is a bit cumbersome but don't what to muddle with strings
    auto transactions = (include_manual) ? storage.get_all<Transaction>(where(between(&Transaction::createdAt, since_rfc, to_rfc)), order_by(&Transaction::createdAt)) :
        storage.get_all<Transaction>(where(between(&Transaction::createdAt, since_rfc, to_rfc) and c(&Transaction::manual) == 0), order_by(&Transaction::createdAt));

    if (print) {
        for (auto& t : transactions)
             fmt::print("{}\n", t.summary());
    }

    return transactions;
}

void create_db() {
    sqlite3* db;

    int rc = sqlite3_open("./info/up-client.db", &db);

    std::string sql = "CREATE TABLE \"Transaction\"("
                      "created_at text primary key, "
                      "amount integer not null, "
                      "description text not null, "
                      "tag text not null, "
                      "manual integer not null)";

    fmt::print("{}\n", sql);
    char* ptr = 0;
    rc = sqlite3_exec(db, sql.c_str(), 0, 0, &ptr);

    sqlite3_close(db);

    std::ifstream csv;
    csv.open("info/data.csv");
    std::string line;

    auto storage = Repository::get_storage();

    Transaction t;
    while (getline(csv, line)) {
        t = Transaction::csv_line_to_transaction(line);

        if (t.description == "vivcourt" || t.description == "rent" || t.description == "reimburse")
            t.manual = true;
        else
            t.manual = false;

        storage.replace(t);
    }
}

void DataManager::AdHoc() {
    create_db();
    auto t = Repository::get_storage().get<Transaction>("2024-06-01T12:48:05+10:00");
    fmt::print("{}\n", t.summary());
}
