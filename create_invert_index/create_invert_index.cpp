#include "common.h"
#include "io.h"

std::vector<int> read_csv(std::string s) {
    std::vector<int> interval;
    std::string delimiter = ",";

    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        interval.push_back(std::stoi(token));
        s.erase(0, pos + delimiter.length());
    }
    return interval;
}

int main() {
    std::unique_ptr<DB_Connector> DB = std::make_unique<DB_Connector>(DB_user, DB_address, DB_password, DB_name, DB_port);
    std::string search_sql = "SELECT * FROM INTERVAL";
    std::unique_ptr<pqxx::result>& R = DB->performQuery(search_sql);
    std::unique_ptr<DB_Connector> DB_invert_index = std::make_unique<DB_Connector>(DB_user, DB_address, DB_password, DB_name, DB_port);
    for (pqxx::result::const_iterator c = R->begin(); c != R->end(); ++c) {
        std::vector<int> interval;
        std::string ID = c[0].as<std::string>();
        int fps = c[2].as<int>();
        std::string interval_str = c[3].as<std::string>();
        interval = read_csv(interval_str);
        for (int i : interval) {
            int interval_by_sec = std::round(1.0 * i / fps);
            if (interval_by_sec <= 3) {
                continue;
            }
            std::string locate_sql = "SELECT * FROM INVERT_INDEX WHERE interval = " + std::to_string(interval_by_sec);
            std::unique_ptr<pqxx::result>& query_result = DB_invert_index->performQuery(locate_sql);
            if (query_result->empty()) {
                std::string db_value = ID + ",";
                std::string insert_sql = std::format("INSERT INTO INVERT_INDEX (INTERVAL,ID) VALUES ({},'{}');", interval_by_sec, db_value);
                DB_invert_index->performQuery(insert_sql);
            }
            else {
                std::string db_value = query_result->begin()[1].as<std::string>();
                db_value += ID + ",";
                std::string insert_sql = std::format("UPDATE INVERT_INDEX SET ID = '{}' WHERE INTERVAL = {};", db_value, interval_by_sec);
                DB_invert_index->performQuery(insert_sql);
            }
        }
    }
    cout << "Operation done successfully" << endl;
}

