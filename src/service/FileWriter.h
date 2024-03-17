

#include "model/Transaction.h"
#include <vector>
class FileWriter {
public:
    FileWriter();
    static void write_to_csv(std::vector<Transaction> transactions);
    static void recalculate_stats();
};
