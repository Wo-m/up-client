

#include "model/Transaction.h"
#include "model/Stats.h"
#include <vector>
class DataManager {
public:
    DataManager();
    static Stats write(std::vector<Transaction> transactions);
    static Stats calculate_stats(std::vector<Transaction> transactions);
    static void recalculate_stats();
    static Stats get_current_stats();
    static void correct_nulls(std::vector<Transaction>& transactions);
};
