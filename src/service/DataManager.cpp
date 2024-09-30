#include "service/DataManager.h"
#include "config/Config.h"
#include "model/Tags.h"
#include "model/Transaction.h"
#include "service/DateHelper.h"
#include "service/Repository.h"
#include "service/UpService.h"
#include <date/date.h>
#include <fmt/core.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <sqlite3.h>
#include <sqlite_orm/sqlite_orm.h>
#include <string>
#include <unordered_set>
#include <vector>

using namespace sqlite_orm;

Stats DataManager::CalculateStats(std::vector<Transaction> transactions)
{
    auto tag_to_amount = std::map<Tag, Amount>();
    for (auto& t : transactions)
    {
        tag_to_amount[t.tag] += t.amount;
    }

    Amount income = tag_to_amount[INCOME];
    Amount expense = 0;

    for (auto& pair : tag_to_amount)
    {
        if (pair.first == INCOME)
            continue;

        expense += pair.second;
    }

    return { income, expense, income + expense, transactions.back().created_at, tag_to_amount };
}

std::vector<Snapshot> DataManager::GenerateSnapshots(int choice)
{
    std::vector<date::year_month_day> dates;
    switch (choice)
    {
        case 1: {
            auto last_mon = DateHelper::GetLastMonday();
            while (last_mon >= DateHelper::ToYearMonthDay(Config::begin))
            {
                dates.insert(dates.begin(), last_mon);
                last_mon = date::year_month_day{ date::sys_days(last_mon) - date::weeks(1) };
            }
            break;
        }
        case 2: {
            auto last_pay = DateHelper::GetLastPay();
            while (last_pay >= DateHelper::ToYearMonthDay(Config::begin))
            {
                dates.insert(dates.begin(), last_pay);
                last_pay = last_pay - date::months(1);
            }
        }
    }

    std::vector<Transaction> transactions;
    std::vector<Snapshot> snaphots;
    Stats stats;
    dates.emplace_back(DateHelper::GetToday());
    for (int i = 1; i < dates.size(); i++)
    {
        auto since = dates.at(i - 1);
        auto to = date::year_month_day{ date::sys_days(dates.at(i)) - date::days(1) };

        transactions = FindTransactions(since, to, false);
        if (transactions.size() == 0)
            continue;

        stats = CalculateStats(transactions);
        snaphots.push_back({since, to, transactions, stats});
    }
    return snaphots;
}

nlohmann::json getCategories()
{
    std::ifstream ifs("info/categories.json");
    return nlohmann::json::parse(ifs);
}

void DataManager::AddTransaction(Transaction& transaction)
{
    auto storage = Repository::get_storage();
    storage.insert(transaction);
}

void DataManager::UpdateTransactions(bool dry_run)
{
    auto storage = Repository::get_storage();

    // testing mode, doesn't actually write anything to db
    if (dry_run)
        storage.begin_transaction();

    auto last_transaction = storage.get_all<Transaction>(where(c(&Transaction::manual) == 0), order_by(&Transaction::created_at).desc(), limit(1));
    auto transactions = up_service_.FindNewTransactions(DateHelper::RFCToYearMonthDay(last_transaction.begin()->created_at));
    if (transactions.empty())
    {
        fmt::print("no new transactions\n");
        return;
    }

    // get all transactions that overlap with fetch, not including manual entries
    auto earliest_transaction_date = DateHelper::RFCToYearMonthDay(transactions.front().created_at);
    auto db_transactions = FindTransactions(earliest_transaction_date, DateHelper::GetToday(), false, false);

    if (db_transactions.empty())
        return;

    // add all values to db and create set of dates
    std::unordered_set<std::string> up_ids;
    for (auto& t : transactions)
    {
        // only insert if its a new transaction
        // otherwise if we manually set tags they get overriden
        // (may want to rethink this if it becomes an issue)
        auto matching_id = storage.select(&Transaction::up_id, where(c(&Transaction::up_id) == t.up_id));
        if (matching_id.empty())
            storage.insert(t);
        up_ids.emplace(t.up_id);
    }

    // remove any values that aren't in set (usually card checks)
    for (auto& t : db_transactions)
    {
        if (up_ids.find(t.up_id) == up_ids.end())
        {
            storage.remove<Transaction>(t.id);
            fmt::print("removing transaction: {}\n", t.summary());
        }
    }

    // we just use this to print
    auto to_rfc = DateHelper::ConvertToRFC(DateHelper::GetToday(), true);
    auto new_transactions = FindTransactions(db_transactions.back().created_at, to_rfc, false, true);

    if (new_transactions.size() > 1)
    {
        new_transactions.erase(new_transactions.begin());    // last known transaction
        fmt::print("NEW TRANSACTIONS\n");
        for (auto& t : new_transactions)
            fmt::print("{}\n", t.summary());
    }
}

std::vector<Transaction> DataManager::FindTransactions(const date::year_month_day& since,
                                                       const date::year_month_day& to,
                                                       bool print,
                                                       bool include_manual)
{
    auto since_rfc = DateHelper::ConvertToRFC(since);
    auto to_rfc = DateHelper::ConvertToRFC(to, true);
    return FindTransactions(since_rfc, to_rfc, print, include_manual);
}

std::vector<Transaction> DataManager::FindTransactions(const std::string& since_rfc, const std::string& to_rfc, bool print, bool include_manual)
{
    auto storage = Repository::get_storage();

    using namespace sqlite_orm;

    // doesn't appear to be a way to prepare a statement and add to it without using strings
    // this is a bit cumbersome but don't what to muddle with strings
    auto transactions = (include_manual)
                            ? storage.get_all<Transaction>(where(between(&Transaction::created_at, since_rfc, to_rfc)),
                                                           order_by(&Transaction::created_at))
                            : storage.get_all<Transaction>(where(between(&Transaction::created_at, since_rfc, to_rfc) and
                                                                 c(&Transaction::manual) == 0),
                                                           order_by(&Transaction::created_at));

    if (print)
    {
        for (auto& t : transactions)
            fmt::print("{}\n", t.summary());
    }

    return transactions;
}

void DataManager::AdHoc()
{
    UpService up;
    auto transactions = up.GetTransactions(up.GetTransactionalAccount().id, DateHelper::ConvertToRFC(Config::begin));
    auto storage = Repository::get_storage();

    for (auto& t : transactions)
    {
        auto db_t = storage.get_all<Transaction>(where(c(&Transaction::created_at) == t.created_at));
        if (db_t.empty())
            continue;
        db_t.front().up_id = t.up_id;
        storage.update(db_t.front());
    }

}
