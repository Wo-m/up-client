//
// Created by Dominic Thompson on 20/7/2023.
//

#pragma once
#include "config/Config.h"
#include "model/Account.h"
#include "model/Tags.h"
#include "model/Transaction.h"
#include "service/DateHelper.h"
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <utility>

class UpService
{
public:
    UpService();

    std::vector<Transaction> FindNewTransactions();
    std::vector<Transaction> GetTransactions(const std::string& accountId, const std::string& since);
    Account GetTransactionalAccount();
    std::vector<Account> GetAccounts();
    void GetCategories();

private:
    nlohmann::json Get(const std::string& path, const cpr::Parameters& params);
    nlohmann::json GetPaged(const std::string& path, const cpr::Parameters& params);
    nlohmann::json Post(const std::string& path, const std::string& body);
    bool SkipTransaction(const nlohmann::json&);
    Tag MapTag(std::string desc, std::string amount);

    // Static Helper Methods
    static cpr::Parameter since(const std::string& date) { return cpr::Parameter{ "filter[since]", date }; }
    static cpr::Parameter until(const std::string& date)
    {
        return cpr::Parameter{ "filter[until]", DateHelper::ConvertToRFC(date) };
    }
    std::vector<Transaction> MapTransactions(const nlohmann::json& transactionsData);

    // Member vars
    std::set<std::string> ignore;                              // descriptions to ignore
    std::map<std::string, std::pair<Tag, std::string>> tag;    // description to tag/price

    // CONSTANTS
    cpr::Parameter PAGE_SIZE = cpr::Parameter{ "page[size]", "100" };
    std::string UP_API = "https://api.up.com.au/api/v1/";
    cpr::Bearer BEARER = { Config::up_api_key };
};
