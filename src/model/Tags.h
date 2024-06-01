#pragma once

#include <sqlite_orm/sqlite_orm.h>
#include <string>

enum Tag {
    EXPECTED,
    ANNA,
    OPAL,
    FLIGHTS,
    BIG,
    INCOME,
    NONE,
};

inline std::string to_string(Tag tag) {
    switch (tag) {
        case EXPECTED:
            return "EXPECTED";
        case ANNA:
            return "ANNA";
        case OPAL:
            return "OPAL";
        case FLIGHTS:
            return "FLIGHTS";
        case BIG:
            return "BIG";
        case INCOME:
            return "INCOME";
        case NONE:
            return "NONE";
    }
    return "NONE";
}

inline Tag tag_from_string(std::string name) {
    if (name == "EXPECTED") return EXPECTED;
    if (name == "ANNA") return ANNA;
    if (name == "OPAL") return OPAL;
    if (name == "FLIGHTS") return FLIGHTS;
    if (name == "BIG") return BIG;
    if (name == "INCOME") return INCOME;

    return NONE;
}

namespace sqlite_orm {

    template<>
    struct type_printer<Tag> : public text_printer {};

    template<>
    struct statement_binder<Tag> {

        int bind(sqlite3_stmt* stmt, int index, const Tag& value) {
            return statement_binder<std::string>().bind(stmt, index, to_string(value));
            //  or return sqlite3_bind_text(stmt, index++, GenderToString(value).c_str(), -1, SQLITE_TRANSIENT);
        }
    };

    template<>
    struct field_printer<Tag> {
        std::string operator()(const Tag& t) const {
            return to_string(t);
        }
    };

    template<>
    struct row_extractor<Tag> {
        Tag extract(const char* row_value) {
            return tag_from_string(row_value);
        }

        Tag extract(sqlite3_stmt* stmt, int columnIndex) {
            auto str = sqlite3_column_text(stmt, columnIndex);
            return this->extract((const char*)str);
        }
    };
}
