//
// Created by Dominic Thompson on 20/7/2023.
//

#include "service/UpService.h"
#include <iostream>

using namespace std;

UpService::UpService() {
}

Account UpService::getTransactionalAccount() {
    return upDao.getTransactionalAccount();
}

void UpService::printTransactionSummary(const std::string &accountId, const std::string &since, const std::string &until) {
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
