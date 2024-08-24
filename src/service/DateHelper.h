#pragma once
#include "config/Config.h"
#include <cstdint>
#include <ctime>
#include <date/date.h>
#include <fmt/core.h>
#include <nlohmann/json.hpp>

using namespace std;

class DateHelper
{
public:
    static date::year_month_day GetLastMonday()
    {
        auto todays_date = date::floor<date::days>(std::chrono::system_clock::now());
        auto monday = todays_date - (date::weekday{ todays_date } - date::Monday);
        return monday;
    }

    static date::year_month_day GetLastPay()
    {
        int pay_date = Config::pay_date;
        auto todays_date = date::floor<date::days>(std::chrono::system_clock::now());
        auto todays_date_ymd = date::year_month_day(todays_date);
        date::year_month_day last_pay;

        if (todays_date_ymd.day() >= date::day(pay_date))
        {
            auto diff = todays_date_ymd.day() - date::day(pay_date);
            last_pay = todays_date - diff;
        }
        else
        {
            auto diff = date::day(pay_date) - todays_date_ymd.day();
            auto last_month = todays_date_ymd - date::months(1);
            last_pay = date::sys_days{ last_month } + diff;
        }

        return date::year_month_day{ last_pay };
    }

    static date::year_month_day GetLastTransactionDate()
    {
        ifstream last_date("info/info.json");
        auto stats = nlohmann::json::parse(last_date);
        return DateHelper::RFCToYearMonthDay(stats["last_date"]);
    }

    static date::year_month_day GetBackdate(date::year_month_day earliest_transaction_date = GetLastTransactionDate())
    {
        // FIXME: use last non manual entry in db
        auto backdate = date::year_month_day{ date::sys_days(earliest_transaction_date) - date::days(Config::backdated_fetch_days) };
        return backdate;
    }

    static date::year_month_day GetToday()
    {
        auto todays_date = date::ceil<date::days>(std::chrono::system_clock::now());
        return todays_date;
    }

    // CONVERTERS ------------------------------------------------------------------------------------------------
    static std::string ConvertToRFC(const std::string& date, bool eod = false)
    {
        return ConvertToRFC(ToYearMonthDay(date), eod);
    }

    static std::string ConvertToRFC(const date::year_month_day date, bool eod = false)
    {
        auto year = (int)date.year();
        auto month = (unsigned int)date.month();
        auto day = (unsigned int)date.day();

        auto static const start_of_day = "00:00:00";
        auto static const end_of_day = "23:59:59.99";

        auto hms = eod ? end_of_day : start_of_day;

        return fmt::format("{}-{:02d}-{:02d}T{}+10:00", year, month, day, hms);
    }

    static date::year_month_day ToYearMonthDay(const std::string& date)
    {
        std::tm time;

        std::stringstream stream(date);
        stream >> std::get_time(&time, "%d/%m/%y");

        auto day = static_cast<uint8_t>(time.tm_mday);
        auto month = static_cast<uint8_t>(time.tm_mon + 1);
        auto year = time.tm_year + 1900;

        // Construct the year_month_day object
        return { date::year{ year }, date::month{ month }, date::day{ day } };
    }

    static date::year_month_day RFCToYearMonthDay(const std::string& rfc)
    {
        std::tm tm = {};
        std::istringstream ss(rfc);
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
        if (ss.fail())
        {
            throw std::runtime_error("Failed to parse date");
        }

        std::ostringstream out;
        out << std::put_time(&tm, "%d/%m/%y");
        return ToYearMonthDay(out.str());
    }

    static std::string ToStringDDMMYY(date::year_month_day date) { return date::format("%d/%m/%y", date); }
};
