#pragma once
#include "config/Config.h"
#include <date/date.h>
#include <fmt/core.h>

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
        auto todays_date = date::floor<date::days>(std::chrono::system_clock::now());
        return todays_date;
    }

    static std::string convertToRFC3339(const std::string& date, bool eod = false) {
        std::string year = date.substr(0, 2);
        std::string month = date.substr(3, 2);
        std::string day = date.substr(6, 2);

        // TODO move these to constants
        //      handle choice as params
        auto start_of_day = "00:00:00";
        auto end_of_day = "23:59:59.99";

        auto hms = eod ? end_of_day : start_of_day;

        return fmt::format("{}-{}-{}T{}+10:00", "20"+year, month, day, hms);
    }
};
