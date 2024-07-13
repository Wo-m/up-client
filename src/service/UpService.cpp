//
// Created by Dominic Thompson on 20/7/2023.
//

#include "service/UpService.h"
#include "service/DateHelper.h"
#include <cpr/cpr.h>
#include <cpr/curl_container.h>
#include <cpr/parameters.h>
#include <cstdio>
#include <exception>
#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <set>
#include <string>
#include <toml.hpp>
#include <utility>
#include <vector>

using namespace std;
using namespace cpr;
using namespace nlohmann;

UpService::UpService()
{
    // TODO: Move to DataManager static
    // build ignore set
    std::ifstream ignore_csv;
    ignore_csv.open("info/ignore.csv");

    std::string line;
    while (!ignore_csv.eof())
    {
        getline(ignore_csv, line);
        try
        {
            ignore.insert(line);
        }
        catch (exception e)
        {
            // eof
        }
    }

    // build tag map
    std::ifstream stream("info/tag.json");
    auto tag_json = nlohmann::json::parse(stream);
    for (auto t : tag_json.items())
    {
        for (auto entry : t.value())
        {
            tag[entry["desc"]] = make_pair(tag_from_string(t.key()), entry["amount"]);
        }
    }
}

vector<Transaction> UpService::FindNewTransactions()
{
    auto account = GetTransactionalAccount();
    auto last_date = DateHelper::GetBackdate();
    string last_date_rfc = DateHelper::ConvertToRFC(last_date);
    return GetTransactions(account.id, last_date_rfc);
}

vector<Transaction> UpService::GetTransactions(const string& accountId, const string& since_)
{
    Parameters parameters = Parameters{ PAGE_SIZE };
    if (since_ != "")
        parameters.Add(since(since_));

    json transactionsData = this->GetPaged("accounts/" + accountId + "/transactions", parameters);

    vector<Transaction> transactions = MapTransactions(transactionsData);

    return transactions;
}

vector<Account> UpService::GetAccounts()
{
    // TODO Cache
    json data = this->Get("accounts", {});

    vector<Account> accounts;

    for (auto& entry : data["data"])
    {
        accounts.push_back({
            entry["id"],
            entry["attributes"]["displayName"],
            stof((string)entry["attributes"]["balance"]["value"]),
            entry["attributes"]["accountType"] == "TRANSACTIONAL",
        });
    }

    return accounts;
}

/**
 * Get Transactional Account
 * @return
 */
Account UpService::GetTransactionalAccount()
{
    auto accounts = GetAccounts();
    auto account = std::find_if(accounts.begin(), accounts.end(), [](const Account& a) { return a.transactional; });
    return *account;
}

// private ------------------------------
/**
 * Get request that supports paged responses
 * @param path
 * @param params
 * @return
 */
json UpService::GetPaged(const std::string& path, const cpr::Parameters& params)
{
    Response r = cpr::Get(Url{ UP_API + path }, BEARER, params);
    cout << r.status_code << " for GET from " << path << endl;

    json data = json::parse(r.text);
    auto next = data["links"]["next"];

    // pagination
    while (next != nullptr)
    {
        r = cpr::Get(Url{ next }, BEARER);
        cout << r.status_code << " for GET from " << path << " page: " << next << endl;

        json nextData = json::parse(r.text);
        data["data"].insert(data["data"].end(), nextData["data"].begin(), nextData["data"].end());

        next = nextData["links"]["next"];
    }

    return data;
}

json UpService::Get(const string& path, const Parameters& params)
{
    Response r = cpr::Get(Url{ UP_API + path }, BEARER, params);
    cout << r.status_code << " for GET from " << path << endl;
    return json::parse(r.text);
}

json UpService::Post(const string& path, const string& body)
{
    Response r = cpr::Post(Url{ UP_API + path }, BEARER, Body{ body });
    cout << r.status_code << " for POST to " << path << endl;
    return json::parse(r.text);
}

vector<Transaction> UpService::MapTransactions(const json& transactionsData)
{
    vector<Transaction> transactions;
    for (auto& transaction : transactionsData["data"])
    {
        if (SkipTransaction(transaction))
            continue;

        auto amount_str = (string)transaction["attributes"]["amount"]["value"];
        amount_str.erase(std::remove(amount_str.begin(), amount_str.end(), '.'));
        auto amount = stoi(amount_str);

        transactions.push_back(
            { amount,
              transaction["attributes"]["description"],
              transaction["attributes"]["createdAt"],
              MapTag(transaction["attributes"]["description"], transaction["attributes"]["amount"]["value"]),
              false });
    };
    return transactions;
}

bool IsInternalTransfer(const json& data)
{
    return !data["relationships"]["transferAccount"]["data"].is_null();
}

bool UpService::SkipTransaction(const json& data)
{
    return ignore.find(data["attributes"]["description"]) != ignore.end() || IsInternalTransfer(data);
}

Tag UpService::MapTag(std::string desc, std::string amount)
{
    std::pair<Tag, std::string> pair;
    try
    {
        pair = tag.at(desc);
    }
    catch (exception e)
    {
        // no matching tag for desc, check if we can apply big tag
        // negative values, so less than
        if (stoi(amount) < Config::big_amount)
        {
            return BIG;
        }
    }

    // either we have an exact match for the amount expected,
    // or the tag is applied to all transactions from merchant
    // regardless of amount
    if (pair.second == amount || pair.second == "0")
        return pair.first;

    return NONE;
}

/**
 * ad hoc to store all sub-categories
 * in a josn file in pairs -> (sub-cat, cat)
 */
void UpService::GetCategories()
{
    Parameters params;
    json data = this->Get("categories/", params);

    data = data["data"];

    json categories;
    int i = 0;
    for (auto& category : data)
    {
        json parent = category["relationships"]["parent"]["data"];
        if (parent.empty())
            continue;

        categories[i++] = json::array({ category["id"], parent["id"] });
    }

    string json_str = to_string(categories);
    ofstream file;
    file.open("info/categories.json");
    file << json_str;
    file.close();
}
