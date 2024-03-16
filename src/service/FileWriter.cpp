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

    std::ofstream last_date;
    last_date.open("last_date.txt");
    auto& last = transactions.at(transactions.size() - 1);
    last_date << last.createdAt;

    last_date.close();
    csv.close();
}
