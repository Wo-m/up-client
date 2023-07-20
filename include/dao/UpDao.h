//
// Created by Dominic Thompson on 16/7/2023.
//

#pragma once
#include "model/Account.h"
#include "model/Transaction.h"
#include <vector>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include "config/Config.h"

class UpDao {
public:
    UpDao();

    Account getTransactionalAccount();
    std::vector<Transaction> getTransactions(const std::string& accountId, const std::string& since, const std::string& until);

private:
    nlohmann::json get(const std::string& path, const cpr::Parameters& params);
    nlohmann::json getPaged(const std::string& path, const cpr::Parameters& params);
    nlohmann::json post(const std::string& path, const std::string& body);

    // Static Helper Methods
    static cpr::Parameter since(const std::string& date) { return cpr::Parameter{"filter[since]", date};}
    static cpr::Parameter until(const std::string& date) { return cpr::Parameter{"filter[until]", date};}
    static std::vector<Transaction> mapTransactions(const nlohmann::json& transactionsData);

    // CONSTANTS
    std::string UP_API = "https://api.up.com.au/api/v1/";
    cpr::Bearer BEARER = {Config::up_api_key};
    cpr::Parameter PAGE_SIZE = cpr::Parameter{"page[size]", "100"};
};

