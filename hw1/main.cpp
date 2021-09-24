#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;

// time_t for 2019-03-30
#define BASE_TIME 1553875200

typedef struct {
    string id, user_id, mission_id, status;
    double correct_count, answered_count, answer_duration;

    // Represent date as "days since BASE_TIME"
    int started_at, finished_at, created_at;
} Record;

typedef struct {
    int answered_count, correct_count;
    double correct_ratio;
} WeekIndicator;

class Indicator {
  public:
    WeekIndicator week_indicators[4];
    double four_week_slope, three_week_slope, two_week_slope,
        mean_correct_ratio;
    int type;
    Indicator() {
        for (int i = 0; i < 4; ++i) {
            week_indicators[i] = {0};
        }
    }
};

ostream &operator<<(ostream &output, const Record &r) {
    output << "id=" << r.id << "\nuser_id=" << r.user_id
           << "\nmission_id=" << r.mission_id << "\nstatus=" << r.status
           << "\ncorrect_count=" << r.correct_count
           << "\nanswered_count=" << r.answered_count
           << "\nanswer_duration=" << r.answer_duration
           << "\nstarted_at=" << r.started_at
           << "\nfinished_at=" << r.finished_at
           << "\ncreated_at=" << r.created_at << "\n";
    return output;
}

inline double round_to_three_digits(double x) {
    return round(x * 1000.0) / 1000.0;
}

int parse_date(const char *s) {
    tm t = {0, 0, 0};
    int year, month, day;
    sscanf(s, "%d-%d-%d", &year, &month, &day);
    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = day;

    return static_cast<int>(difftime(mktime(&t), BASE_TIME) / (86400.0));
}

double
compute_regression_line_slope(const vector<pair<double, double>> &points) {
    double x_bar = 0.0, y_bar = 0.0;
    for (auto &point : points) {
        x_bar += point.first;
        y_bar += point.second;
    }
    x_bar /= static_cast<double>(points.size());
    y_bar /= static_cast<double>(points.size());

    double numerator = 0.0, denominator = 0.0;
    for (auto &point : points) {
        numerator += (point.first - x_bar) * (point.second - y_bar);
        denominator += (point.first - x_bar) * (point.first - x_bar);
    }
    return numerator / denominator;
}

void read_file(const string &filename, vector<Record *> &records) {
    std::fstream fin(filename);
    string line, temp;

    // Skip header
    getline(fin, line);

    while (getline(fin, line)) {
        stringstream line_stream(line);
        Record *record = new Record;

        getline(line_stream, record->id, ',');
        getline(line_stream, record->user_id, ',');
        getline(line_stream, record->mission_id, ',');
        getline(line_stream, record->status, ',');

        // Only preserve records with "Completed" status
        if (record->status != "Completed") {
            delete record;
            continue;
        }

        getline(line_stream, temp, ',');
        record->correct_count = stod(temp);
        getline(line_stream, temp, ',');
        record->answered_count = stod(temp);
        getline(line_stream, temp, ',');
        record->answer_duration = stod(temp);

        getline(line_stream, temp, ',');
        record->started_at = parse_date(temp.c_str());
        getline(line_stream, temp, ',');
        record->finished_at = parse_date(temp.c_str());
        getline(line_stream, temp, ',');
        record->created_at = parse_date(temp.c_str());

        records.push_back(record);
    }

    fin.close();
}

void compute_indicators(const vector<Record *> &records, int date_num,
                        unordered_map<string, Indicator *> &user_indicators) {
    vector<Record *> filtered_records;

    // Filter four week data
    for (auto &record : records) {
        if (record->started_at >= date_num - 28 &&
            record->started_at <= date_num - 1) {
            filtered_records.push_back(record);
            if (user_indicators.find(record->user_id) ==
                user_indicators.end()) {
                user_indicators[record->user_id] = new Indicator;
            }
        }
    }

    // Compute answered_count and correct_count
    for (auto &record : filtered_records) {
        int week_idx = (date_num - record->started_at - 1) / 7;
        user_indicators[record->user_id]
            ->week_indicators[week_idx]
            .answered_count += record->answered_count;
        user_indicators[record->user_id]
            ->week_indicators[week_idx]
            .correct_count += record->correct_count;
    }

    // Compute correct_ratio
    for (auto &item : user_indicators) {
        Indicator *indicator = item.second;
        for (int i = 0; i < 4; ++i) {
            if (indicator->week_indicators[i].answered_count) {
                indicator->week_indicators[i].correct_ratio =
                    static_cast<double>(
                        indicator->week_indicators[i].correct_count) /
                    static_cast<double>(
                        indicator->week_indicators[i].answered_count);
                // Round to third decimal digits
                indicator->week_indicators[i].correct_ratio =
                    round_to_three_digits(
                        indicator->week_indicators[i].correct_ratio);
            }
        }
    }

    // Fill correct_ratio
    for (auto &item : user_indicators) {
        Indicator *indicator = item.second;
        int n = 0;
        double sum = 0.0;
        for (auto &week_indicator : indicator->week_indicators) {
            if (week_indicator.answered_count) {
                ++n;
                sum += week_indicator.correct_ratio;
            }
        }
        double correct_ratio =
            round_to_three_digits(sum / static_cast<double>(n));
        for (int i = 0; i < 4; ++i) {
            if (!(indicator->week_indicators[i].answered_count)) {
                indicator->week_indicators[i].correct_ratio = correct_ratio;
            }
        }
    }

    // Compute three slopes
    vector<pair<double, double>> points;
    for (auto &item : user_indicators) {
        Indicator *indicator = item.second;
        points.clear();
        points.push_back(make_pair(
            -1.0,
            static_cast<double>(indicator->week_indicators[0].answered_count)));
        points.push_back(make_pair(
            -2.0,
            static_cast<double>(indicator->week_indicators[1].answered_count)));
        indicator->two_week_slope = compute_regression_line_slope(points);

        points.push_back(make_pair(
            -3.0,
            static_cast<double>(indicator->week_indicators[2].answered_count)));
        indicator->three_week_slope = compute_regression_line_slope(points);

        points.push_back(make_pair(
            -4.0,
            static_cast<double>(indicator->week_indicators[3].answered_count)));
        indicator->four_week_slope = compute_regression_line_slope(points);
    }
}

void categorize_users_and_output(
    unordered_map<string, Indicator *> &user_indicators, int target) {

    // Categorize users
    for (auto &item : user_indicators) {
        Indicator *indicator = item.second;
        double mean_answered_count = 0.0;
        for (int i = 0; i < 4; ++i) {
            mean_answered_count += static_cast<double>(
                indicator->week_indicators[i].answered_count);
        }
        mean_answered_count /= 4.0;

        int non_negative_slopes = 0;
        if (indicator->two_week_slope >= 0.0)
            ++non_negative_slopes;
        if (indicator->three_week_slope >= 0.0)
            ++non_negative_slopes;
        if (indicator->four_week_slope >= 0.0)
            ++non_negative_slopes;

        int type;
        double t = static_cast<double>(target);
        if (mean_answered_count >= t && non_negative_slopes >= 2) {
            type = 0;
        } else if (mean_answered_count >= t && non_negative_slopes < 2) {
            type = 1;
        } else if (mean_answered_count < t && non_negative_slopes >= 2) {
            type = 2;
        } else {
            type = 3;
        }

        indicator->type = type;
    }

    // Compute mean_correct_ratio
    for (auto &item : user_indicators) {
        Indicator *indicator = item.second;
        indicator->mean_correct_ratio = 0.0;
        for (int i = 0; i < 4; ++i) {
            indicator->mean_correct_ratio +=
                indicator->week_indicators[i].correct_ratio;
        }
        indicator->mean_correct_ratio /= 4.0;
        indicator->mean_correct_ratio =
            round_to_three_digits(indicator->mean_correct_ratio);
    }

    // Compute user_counts and mean correct ratio for users in each category
    int user_counts[4] = {0};
    double user_mean_correct_ratios[4] = {0};
    for (auto &item : user_indicators) {
        Indicator *indicator = item.second;
        ++(user_counts[indicator->type]);
        user_mean_correct_ratios[indicator->type] +=
            indicator->mean_correct_ratio;
    }
    for (int i = 0; i < 4; ++i) {
        if (user_counts[i]) {
            user_mean_correct_ratios[i] =
                floor(user_mean_correct_ratios[i] / user_counts[i] * 100.0);
        }
    }

    // Output
    for (int i = 0; i < 4; ++i) {
        cout << user_counts[i] << ","
             << static_cast<int>(user_mean_correct_ratios[i]) << "\n";
    }
}

int main(int argc, char *argv[]) {
    vector<Record *> records;
    string filename, date;
    int date_num;
    int target;
    cin >> filename >> date >> target;
    date_num = parse_date(date.c_str());
    read_file(filename, records);

    unordered_map<string, Indicator *> user_indicators;
    compute_indicators(records, date_num, user_indicators);
    categorize_users_and_output(user_indicators, target);

    for (auto &item : user_indicators) {
        delete item.second;
    }
    for (Record *record : records) {
        delete record;
    }
    return 0;
}
