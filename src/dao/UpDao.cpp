//
// Created by Dominic Thompson on 16/7/2023.
//

#include "dao/UpDao.h"
#include "config/Config.h"
#include <iostream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <toml.hpp>

using namespace std;
using namespace cpr;
using namespace nlohmann;

UpDao::UpDao() {
    BEARER = Bearer{Config::up_api_key};
}

/**
 * Get transactions for account
 * @param accountId
 * @param since
 * @param until
 * @return
 */
vector<Transaction> UpDao::getTransactions(const string &accountId, const string& since, const string& until) {
    Parameters parameters = Parameters{PAGE_SIZE, {"filter[since]", since}, {"filter[until]", until}};
    basic_json transactionsData = this->get("accounts/" + accountId + "/transactions",parameters);

    vector<Transaction> transactions = std::vector<Transaction>();

    for(;;) {
        addTransactions(transactionsData, transactions);

        // No more pages
        if (transactionsData["links"]["next"] == nullptr) break;

        transactionsData = get(((string) transactionsData["links"]["next"]).erase(0, UP_API.size()), {});
    };

    return transactions;
}

/**
 * Get Transactional Account
 * @return
 */
Account UpDao::getTransactionalAccount() {
    basic_json accountsData = this->get("accounts", Parameters{{"filter[accountType]", "TRANSACTIONAL"}});

    basic_json accountData = accountsData["data"][0];

    Account account =
    {
        accountData["id"],
        accountData["attributes"]["displayName"],
        stof((string) accountData["attributes"]["balance"]["value"])
    };

    return account;
}

// private
json UpDao::get(const string& path, const Parameters& params) {
    Response r = Get(Url{UP_API + path}, BEARER, params);
    cout << r.status_code << " for GET from " << path << endl;
    return json::parse(r.text);
}

json UpDao::post(const string& path, const string& body) {
    Response r =  Post(Url{UP_API + path}, BEARER, Body{body});
    cout << r.status_code << " for POST to " << path << endl;
    return json::parse(r.text);
}


vector<Transaction> UpDao::addTransactions(const json& transactionsData, vector<Transaction>& transactions) {
    for (auto &transaction: transactionsData["data"]) {
        transactions.push_back(
                {
                        transaction["id"],
                        transaction["attributes"]["description"],
                        transaction["relationships"]["category"]["data"] == nullptr ? "null"
                                                                                          : transaction["relationships"]["category"]["data"]["id"],
                        stof((string) transaction["attributes"]["amount"]["value"]),
                        transaction["attributes"]["createdAt"],
                        transaction["attributes"]["settledAt"] == nullptr ? "null"
                                                                          : transaction["attributes"]["settledAt"]
                });
    };

    return transactions;
}
