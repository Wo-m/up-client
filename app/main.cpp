#include <vector>
#include <iostream>
#include "dao/UpDao.h"
#include "config/Config.h"
#include "service/UpService.h"

using namespace std;

int main() {
    // This is just for testing in its current state
    Config();

    UpService upService;
    Account account = upService.getTransactionalAccount();
//    upService.logTransactions(account.id, "2023-06-18T00:00:00+10:00", "2023-07-18T00:00:00+10:00");
    upService.logCategorySummary(account.id, "2023-06-18T00:00:00+10:00", "2023-07-18T00:00:00+10:00");
}