#pragma once

#include "model/Account.h"
#include "model/Transaction.h"
#include "model/Stats.h"
#include <vector>
class DataManager {
public:
    DataManager();
    static void write(std::vector<Transaction> transactions);
    static Stats calculate_stats(std::vector<Transaction> transactions);
    static void correct_nulls(std::vector<Transaction>& transactions);
    static int correct_nulls(Transaction&);
    static void add_new_transaction(Transaction&);
    static void snapshot(int choice, bool);
    static std::vector<Transaction> find_transactions(const std::string&, const std::string&, bool);
    static void calculate_saved(std::vector<Account>);

    static void AdHoc();
};
