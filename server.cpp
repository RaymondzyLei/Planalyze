#include <windows.h>
#include "json.hpp"
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>

using json = nlohmann::json;

bool is_leap_year(int year) {
    if (year % 400 == 0) return 1;
    if (year % 100 == 0) return 0;
    if (year % 4 == 0) return 1;
    return 0;
}
int get_month_day(int year, int month) {
    if (month == 2) return is_leap_year(year) + 28;
    if (month < 8) return 30 + month % 2;
    return 31 - month % 2;
}

std::string to_string(int x, int n = -1) {
    std::string res;
    for (int i = 0; n == -1 ? x : (i < n); ++i) res += char(x % 10 + '0'), x /= 10;
    std::reverse(res.begin(), res.end());
    return res;
}
int to_uint(std::string s) {
    int res = 0;
    for (char c : s) {
        if (!isdigit(c)) return -1;
        res = res * 10 + c - '0';
    }
    return res;
}
std::vector<std::string> split(std::string s, char c) {
    std::vector<std::string> res{""};
    for (auto& x : s) {
        if (x == c) res.push_back("");
        else res.back() += x;
    }
    return res;
}

struct Date {
    int year, month, day;
    static Date parse(std::string s) {
        auto x = split(s, '-');
        if (x.size() != 3) return Date{-1, -1, -1};
        int year = to_uint(x[0]);
        int month = to_uint(x[1]);
        int day = to_uint(x[2]);
        if (year < 1900 || month < 1 || day < 1) return Date{-1, -1, -1};
        if (month > 12 || day > get_month_day(year, month)) return Date{-1, -1, -1};
        return Date{year, month, day};
    }
    std::string dump() {
        if (year < 0) return "-1";
        return to_string(year, 4) + "-" + to_string(month, 2) + "-" + to_string(day, 2);
    }
    bool operator<(const Date& other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        return day < other.day;
    }
    bool operator==(const Date& other) const {
        return year == other.year && month == other.month && day == other.day;
    }
};
struct DateWithoutYear {
    int month, day;
    static DateWithoutYear parse(std::string s) {
        auto x = split(s, '-');
        if (x.size() != 2) return DateWithoutYear{-1, -1};
        int month = to_uint(x[0]);
        int day = to_uint(x[1]);
        if (month < 1 || day < 1) return DateWithoutYear{-1, -1};
        if (month > 12 || day > get_month_day(2000, month)) return DateWithoutYear{-1, -1};
        return DateWithoutYear{month, day};
    }
    std::string dump() {
        if (month < 0) return "-1";
        return to_string(month, 2) + "-" + to_string(day, 2);
    }
    bool operator<(const DateWithoutYear& other) const {
        if (month != other.month) return month < other.month;
        return day < other.day;
    }
    bool operator==(const DateWithoutYear& other) const {
        return month == other.month && day == other.day;
    }
};
struct Time {
    int hour, minute;
    static Time parse(std::string s) {
        auto x = split(s, ':');
        if (x.size() != 2) return Time{-1, -1};
        int hour = to_uint(x[0]);
        int minute = to_uint(x[1]);
        if (hour < 0 || hour > 23 || minute < 0 || minute > 59) return Time{-1, -1};
        return Time{hour, minute};
    }
    std::string dump() {
        if (minute < 0) return "-1";
        return to_string(hour, 2) + ":" + to_string(minute, 2);
    }
    bool operator<(const Time& other) const {
        if (other.hour != hour) return hour < other.hour;
        return minute < other.minute;
    }
};
struct Duration {
    int minute;
    static Duration parse(std::string s) {
        auto x = split(s, ':');
        if (x.size() == 0) {
            return Duration{0};
        }
        if (x.size() == 1) {
            return Duration{to_uint(x[0])};
        }
        if (x.size() == 2) {
            int hour = to_uint(x[0]);
            int minute = to_uint(x[1]);
            if (hour < 0 || minute < 0 || minute > 59) return Duration{-1};
            return Duration{hour * 60 + minute};
        }
        return Duration{-1};
    }
    std::string dump() {
        return to_string(minute / 60) + ":" + to_string(minute % 60, 2);
    }
};

std::tm combine_date_time(Date d, Time t) {
    std::tm res;
    res.tm_year = d.year - 1900;
    res.tm_mon = d.month - 1;
    res.tm_mday = d.day;
    res.tm_hour = t.hour;
    res.tm_min = t.minute;
    res.tm_sec = 0;
    res.tm_isdst = -1;
    return res;
}
std::pair<Date, Time> split_date_time(std::tm t) {
    return {Date{t.tm_year + 1900, t.tm_mon + 1, t.tm_mday}, Time{t.tm_hour, t.tm_min}};
}

int get_weekday(Date d) {
    std::tm t = combine_date_time(d, Time{0, 0});
    std::mktime(&t);
    return t.tm_wday;
}

Time operator+(Time a, const Duration& b) {
    a.minute += b.minute;
    a.hour += a.minute / 60;
    a.minute %= 60;
    a.hour %= 24;
    return a;
}

std::vector<json> events;

std::string read_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return "";
    std::string res;
    while (true) {
        char c = file.get();
        if (file.eof()) break;
        res += c;
    }
    file.close();
    return res;
}
bool write_to_file(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) return true;
    file << content;
    file.close();
    return false;
}

int tot;

void read_events() {
    std::string tmp = read_from_file("data.json");
    if (tmp == "") tmp = "{}";
    json data = json::parse(tmp);
    if (data.size() == 0) {
        data["total"] = 0;
        data["events"] = json::array();
    }
    tot = data["total"];
    events = data["events"];
    std::sort(events.begin(), events.end(), [](const json& a, const json& b) {
        return a["id"] < b["id"];
    });
}

std::string cur_date;
std::string cur_time;
bool new_day;

std::map<std::string, std::vector<std::string>> mp;

void handle_event(json& event) {
    std::string op = "time";
    if (event["type"] == "schedule") op = "start_time";
    if (event["repetition"] == "Once") {
        if (event["date"].get<std::string>() == cur_date) {
            mp[event[op].get<std::string>()].push_back(event["title"].get<std::string>());
        }
    } else if (event["repetition"] == "Custom") {
        if (event["same_time_each_day"]) {
            for (auto e : event["subevents"]) {
                if (e["date"].get<std::string>() == cur_date) {
                    mp[event[op].get<std::string>()].push_back(e["title"].get<std::string>());
                }
            }
        } else {
            for (auto e : event["subevents"]) {
                if (e["date"].get<std::string>() == cur_date) {
                    mp[e[op].get<std::string>()].push_back(e["title"].get<std::string>());
                }
            }
        }
    } else {
        if (event["repetition"] == "Weekly") {
            auto tmp = event["enabled_days"];
            if (!std::binary_search(tmp.begin(), tmp.end(), get_weekday(Date::parse(cur_date)))) return;
        } else if (event["repetition"] == "Monthly") {
            auto tmp = event["enabled_days"];
            if (!std::binary_search(tmp.begin(), tmp.end(), Date::parse(cur_date).day)) return;
        } else if (event["repetition"] == "Yearly") {
            auto tmp = event["enabled_days"];
            if (!std::binary_search(tmp.begin(), tmp.end(), cur_date.substr(5, 5))) return;
        }
        if (cur_date < event["start_date"].get<std::string>() ||
            cur_date > event["end_date"].get<std::string>()) return;
        auto tmp = event["banned"];
        for (auto ban : tmp) {
            if (cur_date <= ban["r"].get<std::string>() &&
                cur_date >= ban["l"].get<std::string>()) return;
        }
        mp[event[op].get<std::string>()].push_back(event["title"].get<std::string>());
    }
}

void check_update() {
    bool need_update = new_day;
    if (read_from_file("update.txt") == "1") {
        read_events();
        write_to_file("update.txt", "0");
        need_update = 1;
    }
    new_day = 0;
    if (!need_update) return;
    mp.clear();
    for (auto& e : events) {
        handle_event(e);
    }
}

const wchar_t* convert(std::string s) {
    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wideStr = converter.from_bytes(s);
    return wideStr.c_str();
}

void handle() {
    check_update();
    std::string res = "";
    if (mp.find(cur_time) == mp.end()) return;
    for (auto& e : mp[cur_time]) {
        res += e + "\n";
    }
    MessageBoxW(NULL, convert(res), convert("Reminder"), MB_OK | MB_TOPMOST | MB_SYSTEMMODAL);
}

int main() {
    time_t now = time(0);
    std::tm ltm = *localtime(&now);
    cur_date = Date{ltm.tm_year + 1900, ltm.tm_mon + 1, ltm.tm_mday}.dump();
    cur_time = Time{ltm.tm_hour, ltm.tm_min}.dump();
    read_events();
    new_day = 1;
    check_update();
    while (true) {
        now = time(0);
        ltm = *localtime(&now);
        int count = 60 - ltm.tm_sec;
        Sleep(count * 1000);
        now = time(0);
        ltm = *localtime(&now);
        cur_date = Date{ltm.tm_year + 1900, ltm.tm_mon + 1, ltm.tm_mday}.dump();
        cur_time = Time{ltm.tm_hour, ltm.tm_min}.dump();
        if (cur_time == "00:00") new_day = 1;
        handle();
    }
}