#pragma once

#include "model/Account.h"
#include "model/Stats.h"
#include "model/Transaction.h"
#include <date/date.h>
#include <string>
#include <vector>
class DataManager
{
public:
    DataManager();
    static Stats CalculateStats(std::vector<Transaction> transactions);
    static void GenerateSnapshots(int choice, bool);
    static void CalculateSaved(std::vector<Account>);

    // everthing here can be moved to sql
    static void UpdateTransactions(std::vector<Transaction> transactions);
    static void AddTransaction(Transaction&);
    static std::vector<Transaction>
    FindTransactions(const date::year_month_day&, const date::year_month_day&, bool, bool include_manual = true);
    static std::vector<Transaction>
    FindTransactions(const std::string&, const std::string&, bool, bool include_manual = true);

    static void AdHoc();
};
