//
// Created by Dominic Thompson on 20/7/2023.
//

#include "service/UpService.h"
#include "model/CategorySummary.h"
#include <iostream>

using namespace std;

UpService::UpService() {
}

Account UpService::getTransactionalAccount() {
    return upDao.getTransactionalAccount();
}

void UpService::logCategorySummary(const std::string &accountId, const std::string &since, const std::string &until) {
    vector<Transaction> transactions = upDao.getTransactions(accountId, since, until);
    map<string, CategorySummary> categories;
    for (auto &transaction : transactions) {
        // generate parent category
        if (categories.find(transaction.category) == categories.end()) {
            categories[transaction.category] = CategorySummary{transaction.category, 0};
        }

        // Update parent amount
        CategorySummary* parent = &categories.find(transaction.category)->second;
        parent->amount += transaction.amount;

        // Update subcategory amount (floats default to 0 in map)
        parent->subAmounts[transaction.subCategory] += transaction.amount;
    }

    for (auto &category : categories) {
        cout << category.second.id << ": " << category.second.amount << endl;
        for (auto &subCategory : category.second.subAmounts) {
            cout << "\t" << subCategory.first << ": " << subCategory.second << endl;
        }
        cout << endl;
    }
}

void UpService::logTransactions(const std::string &accountId, const std::string &since, const std::string &until) {
    vector<Transaction> transactions = upDao.getTransactions(accountId, since, until);
    float expense = 0;
    float income = 0;
    for (auto &transaction : transactions) {
        cout << transaction.summary() << std::endl;
        if (transaction.amount < 0) {
            expense += transaction.amount;
        } else {
            income += transaction.amount;
        }
    }
    cout << "Transaction Count: " << transactions.size() << endl;
    cout << "Total Expense: " << expense << endl;
    cout << "Total Income: " << income << endl;
    cout << "Net: " << income + expense << endl;
}
