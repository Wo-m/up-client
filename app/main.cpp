#include <vector>
#include <iostream>
#include "dao/UpDao.h"
#include "config/Config.h"

using namespace std;

int main() {
    // This is just for testing in its current state
    Config();
    UpDao dao = UpDao();
    Account account = dao.getTransactionalAccount();
    cout << "name: " << account.name << " balance: " << account.balance << endl;
    vector<Transaction> transactions = dao.getTransactions(account.id, "2023-06-18T00:00:00+10:00", "2023-07-18T00:00:00+10:00");

    float total = 0;
    float count = 0;
    for (auto& transaction : transactions) {
        if (transaction.description == "Me (anz)") {
            // don't include me sending money to myself
            continue;
        }
        cout << "Amount " << transaction.amount << " Category: " << transaction.category << " description: " << transaction.description <<  " createdAt: " << transaction.createdAt << endl;
        total += transaction.amount;
        count++;
    }
    cout << "Transaction Count: " << count << endl;
    cout << "Total: " << total << endl;
}