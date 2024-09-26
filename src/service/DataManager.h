#pragma once

#include "model/Account.h"
#include "model/Stats.h"
#include "model/Transaction.h"
#include "service/UpService.h"
#include <date/date.h>
#include <string>
#include <vector>
class DataManager
{
public:
    DataManager();
    static Stats CalculateStats(std::vector<Transaction> transactions);
    static void GenerateSnapshots(int choice, bool);

    // everthing here can be moved to sql
    void UpdateTransactions(bool dry_run);
    static void AddTransaction(Transaction&);
    static std::vector<Transaction>
    FindTransactions(const date::year_month_day&, const date::year_month_day&, bool, bool include_manual = true);
    static std::vector<Transaction>
    FindTransactions(const std::string&, const std::string&, bool, bool include_manual = true);

    static void AdHoc();

private:
    UpService up_service_;
};
