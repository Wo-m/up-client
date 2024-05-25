#pragma once
#include "config/Config.h"
#include <cstdint>
#include <ctime>
#include <date/date.h>
#include <fmt/core.h>
#include <iostream>

using namespace std;

class DateHelper {
public:
    static date::year_month_day get_last_monday() {
        auto todays_date = date::floor<date::days>(std::chrono::system_clock::now());
        auto monday = todays_date - (date::weekday{todays_date} - date::Monday);
        return monday;
    }

    static date::year_month_day get_last_pay() {
        int pay_date = Config::pay_date;
        auto todays_date = date::floor<date::days>(std::chrono::system_clock::now());
        auto todays_date_ymd = date::year_month_day(todays_date);
        date::year_month_day last_pay;

        if (todays_date_ymd.day() >= date::day(pay_date)) {
            auto diff = todays_date_ymd.day() - date::day(pay_date);
            last_pay = todays_date - diff;
        } else {
            auto diff = date::day(pay_date) - todays_date_ymd.day();
            auto last_month = todays_date_ymd - date::months(1);
            last_pay = date::sys_days{last_month} + diff;
        }

        return date::year_month_day{last_pay};
    }

    static date::year_month_day get_today() {
        auto todays_date = date::ceil<date::days>(std::chrono::system_clock::now());
        return todays_date;
    }

    static std::string convertToRFC3339(const std::string& date, bool eod = false) {
        return convertToRFC3339(to_year_month_day(date), eod);
    }

    static std::string convertToRFC3339(const date::year_month_day date, bool eod = false) {
        auto year = (int) date.year();
        auto month = (unsigned int) date.month();
        auto day = (unsigned int) date.day();

        auto static const start_of_day = "00:00:00";
        auto static const end_of_day = "23:59:59.99";

        auto hms = eod ? end_of_day : start_of_day;

        return fmt::format("{}-{:02d}-{}T{}+10:00", year, month, day, hms);
    }

    static date::year_month_day to_year_month_day(const std::string& date) {
        std::tm time;

        std::stringstream stream(date);
        stream >> std::get_time(&time, "%d/%m/%y");

        auto day = static_cast<uint8_t>(time.tm_mday);
        auto month = static_cast<uint8_t>(time.tm_mon + 1);
        auto year = time.tm_year + 1900;

        // Construct the year_month_day object
        return {date::year{year}, date::month{month}, date::day{day}};
    }

    static std::string to_string_yymmdd(date::year_month_day date) {
        return date::format("%d/%m/%y", date);
    }
};
