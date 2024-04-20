//
// Created by Dominic Thompson on 20/7/2023.
//

#include "service/UpService.h"
#include "service/DateHelper.h"
#include <cpr/parameters.h>
#include <cstdio>
#include <exception>
#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json_fwd.hpp>
#include <set>
#include <string>
#include <utility>
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
    // TODO: Move to DataManager static
    // build ignore set
    std::ifstream ignore_csv;
    ignore_csv.open("info/ignore.csv");

    std::string line;
    while (!ignore_csv.eof()) {
        getline(ignore_csv, line);
        try {
            ignore.insert(line);
        } catch (exception e) {
            //eof
        }
    }

    // build tag map
    std::ifstream stream("info/tag.json");
    auto tag_json = nlohmann::json::parse(stream);
    for (auto t : tag_json.items()) {
        for (auto entry : t.value()) {
            tag[entry["desc"]] = make_pair(tag_from_string(t.key()), entry["amount"]);
        }
    }
}

std::string getLastTransactionDate() {
    ifstream last_date("info/info.json");
    auto stats = nlohmann::json::parse(last_date);
    return stats["last_date"];
}

void UpService::logTransactions(const std::string &accountId, const std::string &since, const std::string &until) {
    vector<Transaction> transactions = getTransactions(accountId, since);
    for (auto &transaction : transactions) {
        fmt::print(transaction.summary());
    }
}

vector<Transaction> UpService::find_new_transactions() {
    auto account = getTransactionalAccount();
    string last_date = getLastTransactionDate();
    return getTransactions(account.id, last_date);
}

// TODO this really should be here given
// its not going to the api
std::vector<Transaction> UpService::find_transactions(const std::string &since, bool print) {
    auto since_rfc = DateHelper::convertToRFC3339(since);

    std::ifstream csv;
    csv.open("info/data.csv");
    std::string line;

    auto transactions = std::vector<Transaction>();
    Transaction t;
    while(1) {
        getline(csv, line);
        try {
            t = Transaction::csv_line_to_transaction(line);
        } catch (exception e) {
            break;
        }

        if (t.createdAt < since_rfc) continue;
        if (print) fmt::print("{}\n", t.summary());
        transactions.push_back(t);
    }
    return transactions;
}

/**
 * Get transactions for account
 * @param accountId
 * @param since
 * @param until
 * @return  Config();
 */
vector<Transaction> UpService::getTransactions(const string &accountId, const string& since_) {
    Parameters parameters = Parameters{PAGE_SIZE};
    if (since_ != "")
        parameters.Add(since(since_));
    
    json transactionsData = this->getPaged("accounts/" + accountId + "/transactions",parameters);

    if (transactionsData["data"].empty() || transactionsData["data"].size() == 1) {
        printf("no new transactions\n");
        return {};
    }

    vector<Transaction> transactions = mapTransactions(transactionsData);


    // reverse transactions
    std::reverse(transactions.begin(), transactions.end());
    transactions.erase(transactions.begin());

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

        if (skipTransaction(transaction["attributes"]["description"]))
            continue;

        transactions.push_back(
                {
                        stof((string) transaction["attributes"]["amount"]["value"]),
                        transaction["attributes"]["description"],
                        transaction["relationships"]["parentCategory"]["data"] == nullptr ? "null"
                                                                                    : transaction["relationships"]["parentCategory"]["data"]["id"],
                        transaction["relationships"]["category"]["data"] == nullptr ? "null"
                                                                                    : transaction["relationships"]["category"]["data"]["id"],
                        transaction["attributes"]["createdAt"],
                        map_tag(transaction["attributes"]["description"], transaction["attributes"]["amount"]["value"])
                });
    };
    return transactions;
}

bool UpService::skipTransaction(std::string description) {
    return ignore.find(description) != ignore.end();
}

Tag UpService::map_tag(std::string desc, std::string amount) {
    std::pair<Tag, std::string> pair;
    try {
        pair = tag.at(desc);
    } catch (exception e) {
        return NONE;
    }

    if (pair.second == "0") 
        return pair.first;
    
    if (pair.second == amount)
        return pair.first;

    if (stoi(amount) > Config::big_amount) {
        return BIG;
    }

    return NONE;
}

/**
* ad hoc to store all sub-categories
* in a josn file in pairs -> (sub-cat, cat)
*/
void UpService::getCategories() {
    Parameters params;
    json data = this->get("categories/", params);

    data = data["data"];

    json categories;
    int i = 0;
    for (auto& category : data) {
        json parent = category["relationships"]["parent"]["data"];
        if (parent.empty()) continue;
        
        categories[i++] = json::array({category["id"], parent["id"]});
    }

    string json_str = to_string(categories);
    ofstream file;
    file.open("info/categories.json");
    file << json_str;
    file.close();
}
