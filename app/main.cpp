#include "config/Config.h"
#include "service/UpService.h"
#include "service/FileWriter.h"

using namespace std;

int main() {
    // This is just for testing in its current state
    Config();

    UpService upService;
    Account account = upService.getTransactionalAccount();
    auto transactions = upService.getTransactions(account.id, "15/03/2024",  "");
    FileWriter::write_to_csv(transactions);
}
