//
// Created by Dominic Thompson on 16/7/2023.
//

#pragma once
#include "model/Account.h"
#include "model/Transaction.h"
#include <vector>
#include <cpr/cpr.h>

class UpDao {
public:
    UpDao();
    Account getTransactionalAccount();
    std::vector<Transaction> getTransactions(const std::string& accountId, const std::string& since, const std::string& until);

private:
    cpr::Response get(const std::string& path, const cpr::Parameters&);
    cpr::Response get(const std::string& path);
    cpr::Response post(const std::string& path, const std::string& body);
    // TODO move this key to a config file and dont commit it
    std::string key = "";
    std::string upApi = "https://api.up.com.au/api/v1/";
};

