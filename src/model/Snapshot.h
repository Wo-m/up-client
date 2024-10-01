#include "model/Stats.h"
#include "model/Transaction.h"
#include <date/date.h>
#include <vector>

struct Snapshot
{
    date::year_month_day since;
    date::year_month_day to;
    std::vector<Transaction> transactions;
    Stats stats;
};
