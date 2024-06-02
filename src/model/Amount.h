#pragma once

#include <fmt/format.h>
#include <sqlite_orm/sqlite_orm.h>
#include <string>

/**
 * stores amounts in base value
 * i.e. cents
 *
 * assumes float param is in dollar form when constructing
 * always formats to dollar form
 */
class Amount
{
public:
    Amount() { value_ = 0; }
    Amount(int value) { value_ = value; };
    Amount(float value) { value_ = int(value * 100); };    // 1.00 -> 100;

    Amount& operator+=(const Amount& rhs)
    {
        value_ += rhs.value_;
        return *this;
    }

    friend Amount operator+(Amount lhs, const Amount& rhs)
    {
        lhs += rhs;
        return lhs;
    }

    Amount& operator-=(const Amount& rhs)
    {
        value_ -= rhs.value_;
        return *this;
    }

    friend Amount operator-(Amount lhs, const Amount& rhs)
    {
        lhs -= rhs;
        return lhs;
    }

    int base() const { return value_; }
    float dollars() { return (float)value_ / 100; }    // 100 -> 1.00

private:
    int value_;    // cents
};

// custom formatter that always formats in dollar form
template<>
struct fmt::formatter<Amount> : formatter<float>
{
    auto format(Amount a, format_context& ctx) const { return formatter<float>::format(a.dollars(), ctx); }
};

// comments are by sqlite_orm fellas
// taken from fnc12/sqlite_orm/blob/master/examples/enum_binding.cpp
namespace sqlite_orm {

template<>
struct type_printer<Amount> : public integer_printer
{
};

template<>
struct statement_binder<Amount>
{
    int bind(sqlite3_stmt* stmt, int index, const Amount& value)
    {
        return statement_binder<std::string>().bind(stmt, index, fmt::to_string(value.base()));
        //  or return sqlite3_bind_text(stmt, index++, GenderToString(value).c_str(), -1, SQLITE_TRANSIENT);
    }
};

template<>
struct field_printer<Amount>
{
    std::string operator()(const Amount& a) const { return fmt::to_string(a.base()); }
};

template<>
struct row_extractor<Amount>
{
    Amount extract(const char* row_value) { return Amount(std::stoi(row_value)); }

    Amount extract(sqlite3_stmt* stmt, int columnIndex)
    {
        auto str = sqlite3_column_text(stmt, columnIndex);
        return this->extract((const char*)str);
    }
};
}
