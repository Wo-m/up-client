//
// Created by Dominic Thompson on 20/7/2023.
//

#pragma once
#include "model/Account.h"
#include "model/Transaction.h"
#include "model/Tags.h"
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#include <utility>
#include "config/Config.h"

class UpService {
public:
    UpService();

    void logTransactions(const std::string& accountId, const std::string& since, const std::string& until);
    std::vector<Transaction> find_new_transactions();
    std::vector<Transaction> find_transactions(const std::string &since);
    std::vector<Transaction> getTransactions(const std::string &accountId, const std::string &since);
    Account getTransactionalAccount();
    void getCategories();

private:
    nlohmann::json get(const std::string& path, const cpr::Parameters& params);
    nlohmann::json getPaged(const std::string& path, const cpr::Parameters& params);
    nlohmann::json post(const std::string& path, const std::string& body);
    static std::string convertToRFC3339(const std::string& inputDate);
    bool skipTransaction(std::string description);
    Tag map_tag(std::string desc, std::string amount);

    // Static Helper Methods
    static cpr::Parameter since(const std::string& date) { return cpr::Parameter{"filter[since]", date};}
    static cpr::Parameter until(const std::string& date) { return cpr::Parameter{"filter[until]", convertToRFC3339(date)};}
    std::vector<Transaction> mapTransactions(const nlohmann::json& transactionsData);

    // Member vars
    std::set<std::string> ignore; // descriptions to ignore
    std::map<std::string, std::pair<Tag, std::string>> tag; // description to tag/price

    // CONSTANTS
    cpr::Parameter PAGE_SIZE = cpr::Parameter{"page[size]", "100"};
    std::string UP_API = "https://api.up.com.au/api/v1/";
    cpr::Bearer BEARER = {Config::up_api_key};
};


