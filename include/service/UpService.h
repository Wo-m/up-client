//
// Created by Dominic Thompson on 20/7/2023.
//

#pragma once
#include "model/Account.h"
#include "dao/UpDao.h"


class UpService {
public:
    UpService();

    Account getTransactionalAccount();
    std::vector<Transaction> getTransactions(const std::string &accountId, const std::string &since, const std::string &until);
    void logTransactions(const std::string& accountId, const std::string& since, const std::string& until);
    void logCategorySummary(const std::string& accountId, const std::string& since, const std::string& until);

private:
    UpDao upDao;
    std::vector<Transaction> filterReturnTransfers(std::vector<Transaction> transactions);
    std::string ME_ANZ = "Me (anz)";
    std::string ME = "MR DOMINIC JOHN THOM";
};


