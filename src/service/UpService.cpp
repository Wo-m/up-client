//
// Created by Dominic Thompson on 20/7/2023.
//

#include "service/UpService.h"
#include <iostream>
#include <vector>
#include <cpr/curl_container.h>
#include <iostream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <toml.hpp>

using namespace std;
using namespace cpr;
using namespace nlohmann;

UpService::UpService() {
}

void UpService::logTransactions(const std::string &accountId, const std::string &since, const std::string &until) {
    vector<Transaction> transactions = getTransactions(accountId, since, until);
    for (auto &transaction : transactions) {
        cout << transaction.summary() << std::endl;
    }
}

/**
 * Get transactions for account
 * @param accountId
 * @param since
 * @param until
 * @return
 */
vector<Transaction> UpService::getTransactions(const string &accountId, const string& since_, const string& until_) {
    Parameters parameters = Parameters{PAGE_SIZE};
    if (since_ != "")
        parameters.Add(since(since_));
    if (until_ != "")
        parameters.Add(until(until_));

    json transactionsData = this->getPaged("accounts/" + accountId + "/transactions",parameters);

    vector<Transaction> transactions = mapTransactions(transactionsData);

	// reverse transactions
	std::reverse(transactions.begin(), transactions.end());

    return transactions;
}

/**
 * Get Transactional Account
 * @return
 */
Account UpService::getTransactionalAccount() {
    json accountsData = this->get("accounts", Parameters{{"filter[accountType]", "TRANSACTIONAL"}});

    // should only be one account
    json accountData = accountsData["data"][0];

    Account account =
    {
        accountData["id"],
        accountData["attributes"]["displayName"],
        stof((string) accountData["attributes"]["balance"]["value"])
    };

    return account;
}

// private ------------------------------
/**
 * Get request that supports paged responses
 * @param path
 * @param params
 * @return
 */
json UpService::getPaged(const std::string &path, const cpr::Parameters &params) {
    Response r = Get(Url{UP_API + path}, BEARER, params);
    cout << r.status_code << " for GET from " << path << endl;

    json data = json::parse(r.text);
    auto next = data["links"]["next"];

    // pagination
    while (next != nullptr) {
        r = Get(Url{next}, BEARER);
        cout << r.status_code << " for GET from " << path << " page: " << next << endl;

        json nextData = json::parse(r.text);
        data["data"].insert(data["data"].end(), nextData["data"].begin(), nextData["data"].end());

        next = nextData["links"]["next"];
    }

    return data;
}

json UpService::get(const string& path, const Parameters& params) {
    Response r = Get(Url{UP_API + path}, BEARER, params);
    cout << r.status_code << " for GET from " << path << endl;
    return json::parse(r.text);
}

json UpService::post(const string& path, const string& body) {
    Response r =  Post(Url{UP_API + path}, BEARER, Body{body});
    cout << r.status_code << " for POST to " << path << endl;
    return json::parse(r.text);
}

vector<Transaction> UpService::mapTransactions(const json& transactionsData) {
    vector<Transaction> transactions;
    for (auto &transaction: transactionsData["data"]) {
        transactions.push_back(
                {
                        transaction["id"],
                        stof((string) transaction["attributes"]["amount"]["value"]),
                        transaction["attributes"]["description"],
                        transaction["relationships"]["parentCategory"]["data"] == nullptr ? "null"
                                                                                    : transaction["relationships"]["parentCategory"]["data"]["id"],
                        transaction["relationships"]["category"]["data"] == nullptr ? "null"
                                                                                    : transaction["relationships"]["category"]["data"]["id"],
                        transaction["attributes"]["createdAt"],
                        transaction["attributes"]["settledAt"] == nullptr ? "null"
                                                                          : transaction["attributes"]["settledAt"]
                });
    };
    return transactions;
}
