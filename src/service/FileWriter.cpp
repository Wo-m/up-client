#include "service/FileWriter.h"
#include "model/Transaction.h"
#include <fstream>
#include <ios>
#include <vector>

FileWriter::FileWriter() {
}

void FileWriter::write_to_csv(std::vector<Transaction> transactions) {
    std::ofstream csv;
    csv.open("data.csv", std::ios_base::app);
    for (auto& t : transactions) {
        csv << t.csv_entry();   
    }
    csv.close();
}
