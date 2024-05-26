#pragma once

#include "model/Account.h"
#include "model/Transaction.h"
#include "model/Stats.h"
#include <date/date.h>
#include <vector>
class DataManager {
public:
    DataManager();
    static Stats calculate_stats(std::vector<Transaction> transactions);
    static void snapshot(int choice, bool);
    static void calculate_saved(std::vector<Account>);

    // everthing here can be moved to sql
    static void write(std::vector<Transaction> transactions);
    static void add_new_transaction(Transaction&);
    static std::vector<Transaction> find_transactions(const date::year_month_day&, const date::year_month_day&, bool);

    static void AdHoc();
};
