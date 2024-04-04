

#include "model/Transaction.h"
#include "model/Stats.h"
#include <vector>
class DataManager {
public:
    DataManager();
    static void write(std::vector<Transaction> transactions);
    static Stats calculate_stats(std::vector<Transaction> transactions);
    static void correct_nulls(std::vector<Transaction>& transactions);
};
