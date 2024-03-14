//
// Created by Dominic Thompson on 20/7/2023.
//

#include "service/UpService.h"
#include <iostream>
#include <vector>

using namespace std;

UpService::UpService() {
}

void UpService::logTransactions(const std::string &accountId, const std::string &since, const std::string &until) {
    vector<Transaction> transactions = upDao.getTransactions(accountId, since, until);
    for (auto &transaction : transactions) {
        cout << transaction.summary() << std::endl;
    }
}

vector<Transaction> UpService::getTransactions(const std::string &accountId, const std::string &since, const std::string &until) {
    return upDao.getTransactions(accountId, since, until);
}

Account UpService::getTransactionalAccount() {
    return upDao.getTransactionalAccount();
}
