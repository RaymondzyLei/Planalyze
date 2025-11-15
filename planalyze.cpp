#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <bitset>
#include <set>
#include <windows.h>
#include "json.hpp"

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

void _help_all() {
    printf("  ____   _                       _                  \n");
    printf(" |  _ \\ | |  __ _  _ __    __ _ | | _   _  ____ ___ \n");
    printf(" | |_) || | / _` || '_ \\  / _` || || | | ||_  // _ \\\n");
    printf(" |  __/ | || (_| || | | || (_| || || |_| | / /|  __/\n");
    printf(" |_|    |_| \\__,_||_| |_| \\__,_||_| \\__, |/___|\\___|\n");
    printf("                                    |___/           \n");
    std::cout << "Start your journy with Planalyze right now!" << std::endl;
    std::cout << "Usage: " << std::endl;
    std::cout << "  planalyze [--help|-h] (<Command>)     show help for a command or all commands" << std::endl;
    std::cout << "  planalyze [--add|-a] ...              add a new event" << std::endl;
    std::cout << "  planalyze [--remove|-r] ...           remove events" << std::endl;
    std::cout << "  planalyze [--list|-l] ...             list events" << std::endl;
}
void _help_add() {
    std::cout << "Usage: " << std::endl;
    std::cout << "  planalyze [--add|-a] [--help|-h]                   show help for this command" << std::endl;
    std::cout << "  planalyze [--add|-a] [schedule|point|deadline]     show help for this command" << std::endl;
}
void _help_remove() {
    std::cout << "Usage: " << std::endl;
    std::cout << "  planalyze [--remove|-r] [--help|-h]          show help for this command" << std::endl;
    std::cout << "  planalyze [--remove|-r] <ID>                 remove the event with the given ID" << std::endl;
    std::cout << "  planalyze [--remove|-r] <ID> <DATE>          remove the event with the given ID and date" << std::endl;
    std::cout << "  planalyze [--remove|-r] <ID> <FROM> <TO>     remove the subevent with the given ID and range" << std::endl;
}
void _help_list() {
    std::cout << "Usage: " << std::endl;
    std::cout << "  planalyze [--list|-l] [--help|-h]                 show help for this command" << std::endl;
    std::cout << "  planalyze [--list|-l] [--all|-a]                  show all events" << std::endl;
    std::cout << "  planalyze [--list|-l] [--certain|-c] <ID1,ID2,...>         show the event with the given ID" << std::endl;
    std::cout << "  planalyze [--list|-l] [--detail|-d]               show details of events" << std::endl;
}

void _help(std::string s) {
    if (s == "") _help_all();
    else if (s == "add" || s == "-a" || s == "--add") _help_add();
    else if (s == "remove" || s == "-r" || s == "--remove") _help_remove();
    else if (s == "list" || s == "-l" || s == "--list") _help_list();
    else std::cout << "unknown command: " << s << std::endl;
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
void save_events() {
    json data;
    data["total"] = tot;
    data["events"] = events;
    write_to_file("data.json", data.dump(4));
    write_to_file("update.txt", "1");
}

void help(int argc, char* argv[]) {
    if (argc == 0) {
        _help_all();
    } else {
        _help(argv[0]);
    }
}

std::string read_anything(std::string question) {
    std::string s;
    std::cout << question;
    std::getline(std::cin, s);
    return s;
}

std::string read_something(std::string question, std::string wrong, std::function<bool(std::string&)> check) {
    std::string s;
    std::cout << question;
    std::getline(std::cin, s);
    while (!check(s)) {
        std::cout << wrong;
        std::getline(std::cin, s);
    }
    return s;
}

bool compare(std::string s, std::string base, bool initial_enabled, bool ignore_case) {
    if (ignore_case) {
        for (auto& c : s) c = toupper(c);
        for (auto& c : base) c = toupper(c);
    }
    if (initial_enabled) {
        if (s.size() == 1 && s[0] == base[0]) return 1;
    }
    if (s == base) return 1;
    return 0;
}
std::string read_option(std::string question, std::string wrong, std::vector<std::string> options, bool initial_enabled = true, bool ignore_case = true, int default_option = -1) {
    return read_something(question, wrong, [&](std::string& s) {
        if (default_option >= 0 && s == "") return s = options[default_option], true;
        for (std::string& x : options) {
            if (compare(s, x, initial_enabled, ignore_case)) return s = x, true;
        }
        return false;
    });
}
Date read_date(std::string question = "Date(yyyy-mm-dd): ", std::string wrong = "Invalid date, please enter again(yyyy-mm-dd): ", std::function<bool(Date&)> check = [](Date&){return true;}) {
    return Date::parse(read_something(question, wrong, [&](std::string& s) {
        auto tmp = Date::parse(s);
        return tmp.year != -1 && check(tmp);
    }));
}
DateWithoutYear read_date_without_year(std::string question = "Date(mm-dd): ", std::string wrong = "Invalid date, please enter again(mm-dd): ", std::function<bool(DateWithoutYear&)> check = [](DateWithoutYear&){return true;}) {
    return DateWithoutYear::parse(read_something(question, wrong, [&](std::string& s) {
        auto tmp = DateWithoutYear::parse(s);
        return tmp.month != -1 && check(tmp);
    }));
}
Time read_time(std::string question = "Time(hh:mm): ", std::string wrong = "Invalid time, please enter again(hh:mm): ", std::function<bool(Time&)> check = [](Time&){return true;}) {
    return Time::parse(read_something(question, wrong, [&](std::string& s) {
        auto tmp = Time::parse(s);
        return tmp.hour != -1 && check(tmp);
    }));
}
Duration read_duration(std::string question = "Duration(hh:mm): ", std::string wrong = "Invalid duration, please enter again(hh:mm): ", std::function<bool(Duration&)> check = [](Duration&) { return true; }) {
    return Duration::parse(read_something(question, wrong, [&](std::string& s) {
        auto tmp = Duration::parse(s);
        return tmp.minute != -1 && check(tmp);
    }));
}
bool read_yn(std::string question = "Yes or No(Y/N): ", std::string wrong = "Invalid input, please enter again(Y/N): ") {
    return read_option(question, wrong, {"Yes", "No"}, true, true) == "Yes";
}
int read_int(std::string question = "Integer: ", std::string wrong = "Invalid input, please enter again: ", int min = 1, int max = INT_MAX) {
    return std::stoi(read_something(question, wrong, [&](std::string& s) {
        int x = std::stoi(s);
        return x >= min && x <= max;
    }));
}

void add(int argc, char* argv[]) {
    if (argc == 0) return _help_add();
    std::string argv0(argv[0]);
    if (argv0 == "--help" || argv0 == "-h") {
        _help_add();
        return;
    }
    json new_event;
    if (argv0 == "schedule" || argv0 == "point" || argv0 == "deadline") {
        read_events();
        std::string s;
        new_event["type"] = argv0;
        new_event["title"] = read_anything("Title: ");
        new_event["description"] = read_anything("Description: ");
        new_event["category"] = read_anything("Category: ");
        new_event["priority"] = read_option(
            "Priority([L]ow, [M]edium, [H]igh): ", 
            "Unknown priority, please choose again([L]ow, [M]edium, [H]igh):", 
            {"Low", "Medium", "High"}
        );
        new_event["repetition"] = read_option(
            "Repetition Method([O]nce, [D]aily, [W]eakly, [M]onthly, [Y]early, [C]ustom):",
            "Unknown method, please choose again([O]nce, [D]aily, [W]eakly, [M]onthly, [Y]early, [C]ustom):",
            {"Once", "Daily", "Weekly", "Monthly", "Yearly", "Custom"}
        );
        if (new_event["repetition"] == "Custom") {
            new_event["same_time_each_day"] = read_yn("Same time every day(Y/N): ");
            if (new_event["same_time_each_day"]) {
                if (argv0 == "schedule") {
                    new_event["start_time"] = read_time("Start Time: ").dump();
                    new_event["duration"] = read_time("Duration: ").dump();
                    new_event["end_time"] = (Time::parse(new_event["start_time"]) + Duration::parse(new_event["duration"])).dump();
                } else {
                    new_event["time"] = read_time("Time: ").dump();
                }
                std::cout << "Input the dates of the event(yyyy-mm-dd), ends with 'end': \n";
                std::set<std::string> dates;
                int n = 0;
                while (true) {
                    json tmp;
                    ++n;
                    tmp["date"] = read_something("Date(" + to_string(n) + "): ", "Invalid date, please enter again(" + to_string(n) + "): ", [&](std::string& s) {
                        if (s == "end") return true;
                        auto tmp = Date::parse(s);
                        if (tmp.year == -1) return false;
                        s = tmp.dump();
                        if (dates.count(s)) return false;
                        dates.insert(s);
                        return true;
                    });
                    if (tmp["date"] == "end") break;
                    tmp["completed"] = false;
                    new_event["subevents"].push_back(tmp);
                }
            } else {
                std::set<std::string> dates;
                int n = 0;
                while (true) {
                    json tmp;
                    ++n;
                    tmp["date"] = read_something("Date(" + to_string(n) + "): ", "Invalid date, please enter again(" + to_string(n) + "): ", [&](std::string& s) {
                        if (s == "end") return true;
                        auto tmp = Date::parse(s);
                        if (tmp.year == -1) return false;
                        s = tmp.dump();
                        if (dates.count(s)) return false;
                        dates.insert(s);
                        return true;
                    });
                    if (tmp["date"] == "end") break;
                    if (argv0 == "schedule") {
                        tmp["start_time"] = read_time("Start Time(" + to_string(n) + "): ").dump();
                        tmp["duration"] = read_duration("Duration(" + to_string(n) + "): ").dump();
                        tmp["end_time"] = (Time::parse(tmp["start_time"]) + Duration::parse(tmp["duration"])).dump();
                    } else {
                        tmp["time"] = read_time("Time(" + to_string(n) + "): ").dump();
                    }
                    tmp["completed"] = false;
                    new_event["subevents"].push_back(tmp);
                }
            }
            std::sort(new_event["subevents"].begin(), new_event["subevents"].end(), [](const json& a, const json& b) {
                return a["date"] < b["date"];
            });
            if (new_event["same_time_each_day"]) {
                new_event["subevents"].erase(std::unique(new_event["subevents"].begin(), new_event["subevents"].end(), [](const json& a, const json& b) {
                    return a["date"] == b["date"];
                }), new_event["subevents"].end());
            }
        } 
        if (new_event["repetition"] == "Once") {
            new_event["date"] = read_date("Date: ").dump();
            if (argv0 == "schedule") {
                new_event["start_time"] = read_time("Start time: ").dump();
                new_event["duration"] = read_duration("Duration: ").dump();
                new_event["end_time"] = (Time::parse(new_event["start_time"]) + Duration::parse(new_event["duration"])).dump();
            } else {
                new_event["time"] = read_time("Time: ").dump();
            }
            new_event["completed"] = false;
        }
        if (new_event["repetition"] == "Daily") {
            new_event["start_date"] = read_something("Start date(yyyy-mm-dd, -1 for no start): ", "Invalid date, please enter again(yyyy-mm-dd, -1 for no start): ", [](std::string& s) {
                if (s == "-1") return true;
                if (Date::parse(s).day == -1) return false;
                s = Date::parse(s).dump();
                return true;
            });
            new_event["end_date"] = read_something("End date(yyyy-mm-dd, -1 for no end): ", "Invalid date, please enter again(yyyy-mm-dd, -1 for no end): ", [&](std::string& s) {
                if (s == "-1") return true;
                if (Date::parse(s).day == -1) return false;
                s = Date::parse(s).dump();
                if (new_event["start_time"] != "-1" && s < new_event["start_date"]) return false;
                return true;
            });
            if (argv0 == "schedule") {
                new_event["start_time"] = read_time("Start Time: ").dump();
                new_event["duration"] = read_duration("Duration: ").dump();
                new_event["end_time"] = (Time::parse(new_event["start_time"]) + Duration::parse(new_event["duration"])).dump();
            } else {
                new_event["time"] = read_time("Time: ").dump();
            }
            new_event["completed"] = json::array();
            new_event["banned"] = json::array();
        }
        if (new_event["repetition"] == "Weekly") {
            std::string tmp = read_something("Enabled days(0-6, separated by space, 0 stands for Sunday): ", "Invalid input, please enter again(0-6, separated by space, 0 stands for Sunday): ", [](std::string& s) {
                auto x = split(s, ' ');
                for (auto& y : x) {
                    if (std::stoi(y) < 0 || std::stoi(y) > 6) return false;
                }
                return true;
            });
            auto tmp1 = split(tmp, ' ');
            for (auto& x : tmp1) {
                new_event["enabled_days"].push_back(std::stoi(x));
            }
            std::sort(new_event["enabled_days"].begin(), new_event["enabled_days"].end());
            new_event["enabled_days"].erase(std::unique(new_event["enabled_days"].begin(), new_event["enabled_days"].end()), new_event["enabled_days"].end());
            new_event["start_date"] = read_something("Start date(yyyy-mm-dd, -1 for no start): ", "Invalid date, please enter again(yyyy-mm-dd, -1 for no start): ", [&](std::string& s) {
                if (s == "-1") return true;
                if (Date::parse(s).day == -1) return false;
                s = Date::parse(s).dump();
                return std::binary_search(new_event["enabled_days"].begin(), new_event["enabled_days"].end(), get_weekday(Date::parse(s)));
            });
            new_event["end_date"] = read_something("End date(yyyy-mm-dd, -1 for no end): ", "Invalid date, please enter again(yyyy-mm-dd, -1 for no end): ", [&](std::string& s) {
                if (s == "-1") return true;
                if (Date::parse(s).day == -1) return false;
                s = Date::parse(s).dump();
                if (new_event["start_time"] != "-1" && s < new_event["start_date"]) return false;
                return std::binary_search(new_event["enabled_days"].begin(), new_event["enabled_days"].end(), get_weekday(Date::parse(s)));
            });
            if (argv0 == "schedule") {
                new_event["start_time"] = read_time("Start Time: ").dump();
                new_event["duration"] = read_duration("Duration: ").dump();
                new_event["end_time"] = (Time::parse(new_event["start_time"]) + Duration::parse(new_event["duration"])).dump();
            } else {
                new_event["time"] = read_time("Time: ").dump();
            }
            new_event["completed"] = json::array();
            new_event["banned"] = json::array();
        }
        if (new_event["repetition"] == "Monthly") {
            std::string tmp = read_something("Enabled days(1-31, separated by space): ", "Invalid input, please enter again(1-31, separated by space): ", [](std::string& s) {
                auto x = split(s, ' ');
                for (auto& y : x) {
                    if (std::stoi(y) < 1 || std::stoi(y) > 31) return false;
                }
                return true;
            });
            auto tmp1 = split(tmp, ' ');
            for (auto& x : tmp1) {
                new_event["enabled_days"].push_back(std::stoi(x));
            }
            std::sort(new_event["enabled_days"].begin(), new_event["enabled_days"].end());
            new_event["enabled_days"].erase(std::unique(new_event["enabled_days"].begin(), new_event["enabled_days"].end()), new_event["enabled_days"].end());
            new_event["start_date"] = read_something("Start date(yyyy-mm-dd, -1 for no start): ", "Invalid date, please enter again(yyyy-mm-dd, -1 for no start): ", [&](std::string& s) {
                if (s == "-1") return true;
                if (Date::parse(s).day == -1) return false;
                s = Date::parse(s).dump();
                return std::binary_search(new_event["enabled_days"].begin(), new_event["enabled_days"].end(), Date::parse(s).day);
            });
            new_event["end_date"] = read_something("End date(yyyy-mm-dd, -1 for no end): ", "Invalid date, please enter again(yyyy-mm-dd, -1 for no end): ", [&](std::string& s) {
                if (s == "-1") return true;
                if (Date::parse(s).day == -1) return false;
                s = Date::parse(s).dump();
                if (new_event["start_time"] != "-1" && s < new_event["start_date"]) return false;
                return std::binary_search(new_event["enabled_days"].begin(), new_event["enabled_days"].end(), Date::parse(s).day);
            });
            if (argv0 == "schedule") {
                new_event["start_time"] = read_time("Start Time: ").dump();
                new_event["duration"] = read_duration("Duration: ").dump();
                new_event["end_time"] = (Time::parse(new_event["start_time"]) + Duration::parse(new_event["duration"])).dump();
            } else {
                new_event["time"] = read_time("Time: ").dump();
            }
            new_event["completed"] = json::array();
            new_event["banned"] = json::array();
        }
        if (new_event["repetition"] == "Yearly") {
            std::string tmp = read_something("Enabled days(1-1 to 12-31): ", "Invalid input, please enter again(1-1 to 12-31): ", [](std::string& s) {
                auto x = split(s, ' ');
                for (auto& y : x) {
                    if (DateWithoutYear::parse(y).day == -1) return false;
                }
                return true;
            });
            auto tmp1 = split(tmp, ' ');
            for (auto& x : tmp1) {
                new_event["enabled_days"].push_back(DateWithoutYear::parse(x).dump());
            }
            std::sort(new_event["enabled_days"].begin(), new_event["enabled_days"].end());
            new_event["enabled_days"].erase(std::unique(new_event["enabled_days"].begin(), new_event["enabled_days"].end()), new_event["enabled_days"].end());
            new_event["start_date"] = read_something("Start date(yyyy-mm-dd, -1 for no start): ", "Invalid date, please enter again(yyyy-mm-dd, -1 for no start): ", [&](std::string& s) {
                if (s == "-1") return true;
                if (Date::parse(s).day == -1) return false;
                s = Date::parse(s).dump();
                return std::binary_search(new_event["enabled_days"].begin(), new_event["enabled_days"].end(), s.substr(5, 5));
            });
            new_event["end_date"] = read_something("End date(yyyy-mm-dd, -1 for no end): ", "Invalid date, please enter again(yyyy-mm-dd, -1 for no end): ", [&](std::string& s) {
                if (s == "-1") return true;
                if (Date::parse(s).day == -1) return false;
                s = Date::parse(s).dump();
                if (new_event["start_time"] != "-1" && s < new_event["start_date"]) return false;
                return std::binary_search(new_event["enabled_days"].begin(), new_event["enabled_days"].end(), s.substr(5, 5));
            });
            if (argv0 == "schedule") {
                new_event["start_time"] = read_time("Start Time: ").dump();
                new_event["duration"] = read_duration("Duration: ").dump();
                new_event["end_time"] = (Time::parse(new_event["start_time"]) + Duration::parse(new_event["duration"])).dump();
            } else {
                new_event["time"] = read_time("Time: ").dump();
            }
            new_event["completed"] = json::array();
            new_event["banned"] = json::array();
        }
        new_event["id"] = ++tot;
        events.push_back(new_event);
        save_events();
    } else {
        std::cout << "Unknown command.\n";
        _help_add();
    }
}
Date add_days(Date d, int days) {
    std::tm t = combine_date_time(d, Time{0, 0});
    t.tm_mday += days;
    std::mktime(&t);
    return Date{t.tm_year + 1900, t.tm_mon + 1, t.tm_mday};
}
json merge_ban_intervals(json x) {
    auto next_date = [&](const std::string& s) {
        if (x["repetition"] == "Daily") {
            return add_days(Date::parse(s), 1).dump();
        }
        if (x["repetition"] == "Weekly") {
            auto d = Date::parse(s);
            auto w = get_weekday(d);
            auto days = x["enabled_days"];
            auto it = ++std::lower_bound(days.begin(), days.end(), w);
            if (it == days.end()) it = days.begin();
            return add_days(d, (it->get<int>() - w + 7) % 7).dump();
        }
        if (x["repetition"] == "Monthly") {
            auto d = Date::parse(s);
            auto days = x["enabled_days"];
            while (true) {
                auto it = ++std::lower_bound(days.begin(), days.end(), d.day);
                if (it == days.end() || it->get<int>() > get_month_day(d.year, d.month)) {
                    ++d.month;
                    d.day = 1;
                    if (d.month > 12) {
                        d.month = 1;
                        ++d.year;
                    }
                } else {
                    return Date{d.year, d.month, it->get<int>()}.dump();
                }
            }
        }
        if (x["repetition"] == "Yearly") {
            auto d = Date::parse(s);
            auto days = x["enabled_days"];
            while (true) {
                auto it = ++std::lower_bound(days.begin(), days.end(), DateWithoutYear{d.month, d.day}.dump());
                if (it == days.end() || DateWithoutYear::parse(it->get<std::string>()).day > get_month_day(d.year, DateWithoutYear::parse(*it).month)) {
                    ++d.year;
                } else {
                    return Date{d.year, DateWithoutYear::parse(it->get<std::string>()).month, DateWithoutYear::parse(it->get<std::string>()).day}.dump();
                }
            }
        }
        std::cout << "Unknown error occured\n";
        exit(1);
        return std::string("-1");
    };
    auto tmp = x["banned"];
    std::sort(tmp.begin(), tmp.end(), [](json& a, json& b) {
        if (a["l"] == "-1") return true;
        return a["l"] < b["l"];
    });
    json res = json::array();
    for (auto& e : tmp) {
        if (!res.empty() && res.back()["r"] == "-1") break;
        if (res.empty() || (e["l"] != "-1" && e["l"].get<std::string>() > next_date(res.back()["r"]))) {
            res.push_back(e);
        } else {
            if (e["r"] == "-1" || e["r"] > res.back()["r"]) res.back()["r"] = e["r"];
        }
    }
    return res;
}
void remove(int argc, char* argv[]) {
    if (argc == 0) return _help_remove();
    std::string argv0 = argv[0];
    if (argv0 == "-h" || argv0 == "--help") return _help_remove();
    read_events();
    int id = to_uint(argv0);
    if (id < 0) {
        std::cout << "Invalid event id.\n";
        return;
    }
    auto it = std::lower_bound(events.begin(), events.end(), id, [](json& a, int b) {
        return a["id"] < b;
    });
    if (it == events.end() || it->at("id") != id) {
        std::cout << "Event not found.\n";
        return;
    }
    if (argc == 1) {
        events.erase(it);
        save_events();
        return;
    }
    auto& e = *it;
    if (e["repetition"] == "Once") {
        events.erase(it);
        save_events();
        return;
    }
    std::string s1, s2;
    Date date1, date2;
    if (argc == 2) {
        s1 = argv[1];
        date1 = Date::parse(s1);
        if (date1.day == -1 && s1 != "-1") {
            std::cout << "Invalid date(yyyy-mm-dd).\n";
            return;
        }
        if (s1 != "-1") s1 = date1.dump();
        s2 = s1;
        date2 = date1;
    }
    if (argc >= 3) {
        s1 = argv[1];
        s2 = argv[2];
        date1 = Date::parse(s1);
        date2 = Date::parse(s2);
        if ((date1.day == -1 && s1 != "-1") || (date2.day == -1 && s2 != "-1")) {
            std::cout << "Invalid date(yyyy-mm-dd).\n";
            return;
        }
        if (s1 != "-1") s1 = date1.dump();
        if (s2 != "-1") s2 = date2.dump();
    }
    if (s1 != "-1" && s2 != "-1" && s1 > s2) {
        std::cout << "Left date should be earlier than right date.\n";
        return;
    }
    if (e["repetition"] == "Custom") {
        std::sort(e["subevents"].begin(), e["subevents"].end(), [](json& a, json& b) {
            return a["date"] < b["date"];
        });
        if (s1 == "-1") s1 = e["subevents"][0]["date"];
        if (s2 == "-1") s2 = e["subevents"].back()["date"];
        auto it1 = std::lower_bound(e["subevents"].begin(), e["subevents"].end(), s1, [](json& a, const std::string& b) {
            return a["date"] < b;
        });
        if (it1 == e["subevents"].end() || it1->at("date") != s1) {
            if (argc == 3) std::cout << "Left date not found.\n";
            else std::cout << "Date not found.\n";
            return;
        }
        auto it2 = std::upper_bound(e["subevents"].begin(), e["subevents"].end(), s2, [](const std::string& a, json& b) {
            return a < b["date"];
        });
        if (it2 == e["subevents"].begin() || prev(it2)->at("date") != s2) {
            std::cout << "Right date not found.\n";
            return;
        }
        for (auto subit = it1; subit != it2; ++subit) {
            e["subevents"].erase(subit);
        }
        if (e["subevents"].empty()) {
            events.erase(it);
        }
        save_events();
        return;
    }
    if (s1 == "-1") s1 = e["start_date"], date1 = Date::parse(s1);
    if (s2 == "-1") s2 = e["end_date"], date2 = Date::parse(s2);
    if (s1 > s2) {
        std::cout << "Left date should be earlier than right date.\n";
        return;
    }
    if (e["start_date"] != "-1" && s1 < e["start_date"]) {
        if (argc == 2) std::cout << "Date not found\n";
        else std::cout << "Left date not found.\n";
        return;
    }
    if (e["end_date"] != "-1" && s2 > e["end_date"]) {
        if (argc == 2) std::cout << "Date not found\n";
        else std::cout << "Right date not found.\n";
        return;
    }
    if (e["repetition"] == "Daily") {
        e["banned"].push_back(json{{"l", s1}, {"r", s2}});
        e["banned"] = merge_ban_intervals(e);
        save_events();
        return;
    }
    if (e["repetition"] == "Weekly") {
        if (s1 != "-1" && !std::binary_search(e["enabled_days"].begin(), e["enabled_days"].end(), get_weekday(date1))) {
            if (argc == 2) std::cout << "Date not found\n";
            else std::cout << "Left date not found.\n";
            return;
        }
        if (s2 != "-1" && !std::binary_search(e["enabled_days"].begin(), e["enabled_days"].end(), get_weekday(date2))) {
            if (argc == 2) std::cout << "Date not found\n";
            else std::cout << "Right date not found.\n";
            return;
        }
        e["banned"].push_back(json{{"l", s1}, {"r", s2}});
        e["banned"] = merge_ban_intervals(e);
        if (e["banned"].size() == 1 && e["banned"][0] == json{{"l", e["start_date"]}, {"r", e["end_date"]}}) {
            events.erase(it);
        }
        save_events();
        return;
    }
    if (e["repetition"] == "Monthly") {
        if (s1 != "-1" && !std::binary_search(e["enabled_days"].begin(), e["enabled_days"].end(), date1.day)) {
            if (argc == 2) std::cout << "Date not found\n";
            else std::cout << "Left date not found.\n";
            return;
        }
        if (s2 != "-1" && !std::binary_search(e["enabled_days"].begin(), e["enabled_days"].end(), date2.day)) {
            if (argc == 2) std::cout << "Date not found\n";
            else std::cout << "Right date not found.\n";
            return;
        }
        e["banned"].push_back(json{{"l", s1}, {"r", s2}});
        e["banned"] = merge_ban_intervals(e);
        if (e["banned"].size() == 1 && e["banned"][0] == json{{"l", e["start_date"]}, {"r", e["end_date"]}}) {
            events.erase(it);
        }
        save_events();
        return;
    }
    if (e["repetition"] == "Yearly") {
        if (s1 != "-1" && !std::binary_search(e["enabled_days"].begin(), e["enabled_days"].end(), date1.dump().substr(5, 5))) {
            if (argc == 2) std::cout << "Date not found\n";
            else std::cout << "Left date not found.\n";
            return;
        }
        if (s2 != "-1" && !std::binary_search(e["enabled_days"].begin(), e["enabled_days"].end(), date2.dump().substr(5, 5))) {
            if (argc == 2) std::cout << "Date not found\n";
            else std::cout << "Right date not found.\n";
            return;
        }
        e["banned"].push_back(json{{"l", s1}, {"r", s2}});
        e["banned"] = merge_ban_intervals(e);
        if (e["banned"].size() == 1 && e["banned"][0] == json{{"l", e["start_date"]}, {"r", e["end_date"]}}) {
            events.erase(it);
        }
        save_events();
        return;
    }
    std::cout << "Unknown error occurred.\n";
}
void _list_detail(json& e) {
    std::cout << "ID: " << e["id"] << "\n";
    std::cout << "Title: " << e["title"].get<std::string>() << "\n";
    std::cout << "Description: " << e["description"].get<std::string>() << "\n";
    std::cout << "Type: " << e["type"].get<std::string>() << "\n";
    std::cout << "Priority: " << e["priority"].get<std::string>() << '\n';
    std::cout << "Repetition: " << e["repetition"].get<std::string>() << '\n';
    if (e["repetition"] == "Once") {
        std::cout << "Date: " << e["date"].get<std::string>() << '\n';
        if (e["type"] == "schedule") {
            std::cout << "From " << e["start_time"].get<std::string>() << " to ";
            std::cout << (Time::parse(e["start_time"]) + Duration::parse(e["duration"])).dump() << '\n';
        } else {
            std::cout << "Time: " << e["time"].get<std::string>() << '\n';
        }
    }
    if (e["repetition"] == "Custom") {
        std::cout << "Subevents:\n";
        if (e["same_time_each_day"]) {
            for (auto& sube : e["subevents"]) {
                std::cout << "  Date: " << sube["date"] << "\n";
                if (e["type"] == "schedule") {
                    std::cout << "From " << e["start_time"].get<std::string>() << " to ";
                    std::cout << (Time::parse(e["start_time"]) + Duration::parse(e["duration"])).dump() << '\n';
                } else {
                    std::cout << "Time: " << e["time"].get<std::string>() << '\n';
                }
            }
        } else {
            for (auto& sube : e["subevents"]) {
                std::cout << "  Date: " << sube["date"].get<std::string>() << "\n";
                if (e["type"] == "schedule") {
                    std::cout << "From " << sube["start_time"].get<std::string>() << " to ";
                    std::cout << (Time::parse(sube["start_time"]) + Duration::parse(sube["duration"])).dump() << '\n';
                } else {
                    std::cout << "Time: " << sube["time"].get<std::string>() << '\n';
                }
            }
        }
    }
    if (e["repetition"] != "Once" && e["repetition"] != "Custom") {
        if (e["repetition"] != "Daily") {
            std::cout << "Enabled days: ";
            for (auto& day : e["enabled_days"]) {
                if (e["repetition"] != "Yearly") std::cout << day.get<int>() << " ";
                else std::cout << day.get<std::string>() << " ";
            }
            std::cout << '\n';
        }
        std::cout << "Start date: " << e["start_date"].get<std::string>() << '\n';
        std::cout << "End date: " << e["end_date"].get<std::string>() << '\n';
        if (e["type"] == "schedule") {
            std::cout << "From " << e["start_time"].get<std::string>() << " to ";
            std::cout << (Time::parse(e["start_time"]) + Duration::parse(e["duration"])).dump() << '\n';
        } else {
            std::cout << "Time: " << e["time"].get<std::string>() << '\n';
        }
    }
    std::cout << "------------------\n";
}
void _list_brief(json& e) {
    std::cout << e["id"] << " " << e["title"] << " " << e["type"] << " " << e["priority"] << " " << e["repetition"] << "\n";
}
void list(int argc, char* argv[]) {
    if (argc == 0) return _help_list();
    std::string argv0 = argv[0];
    if (argv0 == "-h" || argv0 == "--help") return _help_list();
    read_events();
    bool all = false, detail = false;
    std::set<int> ids;
    for (int i = 0; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-a" || arg == "--all") {
            if (ids.size()) {
                std::cout << "Cannot use -a and -c together.\n";
                return;
            }
            all = true;
        } else if (arg == "-d" || arg == "--detail") {
            detail = true;
        } else if (arg == "-c" || arg == "--certain") {
            if (all) {
                std::cout << "Cannot use -c and -a together.\n";
                return;
            }
            if (i + 1 >= argc) {
                std::cout << "Please specify the event id.\n";
                return;
            }
            auto x = split(std::string(argv[i + 1]), ',');
            for (auto& s : x) {
                if (to_uint(s) < 0) {
                    std::cout << "Invalid event id.\n";
                    return;
                } else {
                    ids.insert(to_uint(s));
                }
            }
            i += 1;
        } else {
            std::cout << "Invalid argument: " << arg << "\n";
        }
    }
    if (all) {
        for (auto& e : events) {
            if (detail) _list_detail(e);
            else _list_brief(e);
        }
    } else {
        for (auto& e : events) {
            if (ids.count(e["id"])) {
                ids.erase(e["id"]);
                if (detail) _list_detail(e);
                else _list_brief(e);
            }
        }
        if (ids.size()) {
            std::cout << "Event(s) not found: ";
            for (auto& id : ids) {
                std::cout << id << " ";
            }
            std::cout << "\n";
        }
    }
}

int main(int argc, char* argv[]) {
    system("chcp 65001");
    if (argc == 1) {
        help(argc - 1, argv + 1);
        return 0;
    }
    std::string s = argv[1];
    if (s == "-h" || s == "--help") {
        help(argc - 2, argv + 2);
        return 0;
    }
    if (s == "-a" || s == "--add") {
        add(argc - 2, argv + 2);
        return 0;
    }
    if (s == "-r" || s == "--remove") {
        remove(argc - 2, argv + 2);
        return 0;
    }
    if (s == "-l" || s == "--list") {
        list(argc - 2, argv + 2);
        return 0;
    }
}