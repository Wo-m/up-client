//
// Created by Dominic Thompson on 20/7/2023.
//

#pragma once
#include "model/Account.h"
#include "model/Transaction.h"
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#include "config/Config.h"

class UpService {
public:
    UpService();

    void logTransactions(const std::string& accountId, const std::string& since, const std::string& until);
    std::vector<Transaction> find_new_transactions();
    std::vector<Transaction> getTransactions(const std::string &accountId, const std::string &since);
    Account getTransactionalAccount();
    void getCategories();

private:
    std::vector<Transaction> filterReturnTransfers(std::vector<Transaction> transactions);
    nlohmann::json get(const std::string& path, const cpr::Parameters& params);
    nlohmann::json getPaged(const std::string& path, const cpr::Parameters& params);
    nlohmann::json post(const std::string& path, const std::string& body);
    static std::string convertToRFC3339(const std::string& inputDate);
    bool skipTransaction(std::string description);

    // Static Helper Methods
    static cpr::Parameter since(const std::string& date) { return cpr::Parameter{"filter[since]", date};}
    static cpr::Parameter until(const std::string& date) { return cpr::Parameter{"filter[until]", convertToRFC3339(date)};}
    std::vector<Transaction> mapTransactions(const nlohmann::json& transactionsData);

    // CONSTANTS
    cpr::Parameter PAGE_SIZE = cpr::Parameter{"page[size]", "100"};
    std::string UP_API = "https://api.up.com.au/api/v1/";
    cpr::Bearer BEARER = {Config::up_api_key};
    std::string ME_ANZ = "Me (anz)";
    std::string ME = "MR DOMINIC JOHN THOM";
};


