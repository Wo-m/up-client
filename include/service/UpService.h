//
// Created by Dominic Thompson on 20/7/2023.
//




#include "model/Account.h"
#include "model/Transaction.h"
#include "dao/UpDao.h"


class UpService {
public:
    UpService();

    Account getTransactionalAccount();
    void printTransactionSummary(const std::string& accountId, const std::string& since, const std::string& until);

private:
    UpDao upDao;
};


