

#include "model/Transaction.h"
#include "model/Stats.h"
#include <vector>
class DataManager {
public:
    DataManager();
    static Stats write(std::vector<Transaction> transactions);
    static void recalculate_stats();
    static Stats calculate_stats(std::vector<Transaction> transactions);
};
