//
// Created by Dominic Thompson on 16/7/2023.
//

#include "dao/UpDao.h"
#include <iostream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

using namespace std;
using namespace cpr;
using namespace nlohmann;

UpDao::UpDao() {
// Nothing
}

/**
 * Get transactions for account
 * @param accountId
 * @param since
 * @param until
 * @return
 */
vector<Transaction> UpDao::getTransactions(const string &accountId, const string& since, const string& until) {
    Parameters parameters = Parameters{{"page[size]", "100"}, {"filter[since]", since}, {"filter[until]", until}};

    Response r = this->get("accounts/" + accountId + "/transactions",
                           parameters);
    cout << "Up HTTP Response: " << r.status_code << endl;

    basic_json transactionsData = json::parse(r.text);

    vector<Transaction> transactions = std::vector<Transaction>();

    for (auto& transaction : transactionsData["data"]) {
        transactions.push_back(
                {
                    transaction["id"],
                    transaction["attributes"]["description"],
                    transaction["relationships"]["category"]["data"]["id"] == nullptr ? "null" : transaction["relationships"]["category"]["data"]["id"],
                    stof((string) transaction["attributes"]["amount"]["value"]),
                    transaction["attributes"]["createdAt"],
                    transaction["attributes"]["settledAt"] == nullptr ? "null" : transaction["attributes"]["settledAt"]
                });
    };

    return transactions;
}

/**
 * Get Transactional Account
 * @return
 */
Account UpDao::getTransactionalAccount() {

    // Process Get Request
    Response r = this->get("accounts", Parameters{{"filter[accountType]", "TRANSACTIONAL"}});
    cout << "Up HTTP Response: " << r.status_code << endl;

    // Convert Response to JSON
    basic_json accountsData = json::parse(r.text);

    // Only 1 transactional Account
    basic_json accountData = accountsData["data"][0];

    return {accountData["id"],
            accountData["attributes"]["displayName"],
            stof((string) accountData["attributes"]["balance"]["value"])};
}

// private
Response UpDao::get(const string& path) {
    return Get(Url{upApi + path},
               Bearer{this->key});
}

Response UpDao::get(const string& path, const Parameters& params) {
    return Get(Url{upApi + path},
               Bearer{this->key},
               params);
}

Response UpDao::post(const string& path, const string& body) {
    return Post(Url{upApi + path},
                Bearer{this->key},
                Body{body});
}
