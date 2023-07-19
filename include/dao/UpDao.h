//
// Created by Dominic Thompson on 16/7/2023.
//

#pragma once
#include "model/Account.h"
#include "model/Transaction.h"
#include <vector>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

class UpDao {
public:
    UpDao();
    Account getTransactionalAccount();
    std::vector<Transaction> getTransactions(const std::string& accountId, const std::string& since, const std::string& until);

private:
    nlohmann::json get(const std::string& path, const cpr::Parameters& params);
    nlohmann::json post(const std::string& path, const std::string& body);
    static std::vector<Transaction> addTransactions(const nlohmann::json& transactionsData, std::vector<Transaction>& transactions);

    cpr::Bearer BEARER = {""};

    // CONSTANTS
    std::string UP_API = "https://api.up.com.au/api/v1/";
    cpr::Parameter PAGE_SIZE = cpr::Parameter{"page[size]", "100"};
};

