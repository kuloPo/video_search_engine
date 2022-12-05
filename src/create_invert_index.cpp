/*
 * This file is part of kuloPo/video_search_engine.
 * Copyright (c) 2021-2022 Wen Kang, Alberto Krone-Martins
 *
 * kuloPo/video_search_engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * kuloPo/video_search_engine is distributed in the hope that it will be useful,but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kuloPo/video_search_engine. If not, see <https://www.gnu.org/licenses/>.
 */

#include "common.h"
#include "io.h"
#include "utils.h"

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

std::string form_update_sql(const std::string& ID, const double interval_by_sec, const std::string& existing_value) {
    std::string update_sql = "";
    if (existing_value.empty()) {
        std::string db_value = ID + ",";
        update_sql += "INSERT INTO INVERT_INDEX (INTERVAL,ID) VALUES (";
        update_sql += std::to_string(interval_by_sec);
        update_sql += ",'";
        update_sql += db_value;
        update_sql += "');";
    }
    else {
        std::string db_value = existing_value + ID + ",";
        update_sql += "UPDATE INVERT_INDEX SET ID = '";
        update_sql += db_value;
        update_sql += "' WHERE INTERVAL = ";
        update_sql += std::to_string(interval_by_sec);
        update_sql += ";";
    }
    return update_sql;
}

int main() {
    read_config();
    // create 2 DB Connector, one for interval table and one for invert index table
    std::unique_ptr<DB_Connector> DB = std::make_unique<DB_Connector>(DB_user, DB_address, DB_password, DB_name, DB_port);
    std::string search_sql = "SELECT * FROM INTERVAL";
    std::unique_ptr<pqxx::result> R = DB->performQuery(search_sql);
    std::unique_ptr<DB_Connector> DB_invert_index = std::make_unique<DB_Connector>(DB_user, DB_address, DB_password, DB_name, DB_port);
    // for each video in interval table
    for (pqxx::result::const_iterator c = R->begin(); c != R->end(); ++c) {
        std::vector<int> interval;
        std::vector<int> dup_guard;
        std::string ID = c[0].as<std::string>();
        int fps = c[2].as<int>();
        std::string interval_str = c[3].as<std::string>();
        interval = read_csv(interval_str);
        for (int i : interval) {
            int interval_by_sec = std::round(1.0 * i / fps);
            // drop too short intervals 
            if (interval_by_sec <= 3) {
                continue;
            }
            if (vector_contain(dup_guard, interval_by_sec)) {
                continue;
            }
            std::string locate_sql = "SELECT * FROM INVERT_INDEX WHERE interval = " + std::to_string(interval_by_sec);
            std::unique_ptr<pqxx::result> query_result = DB_invert_index->performQuery(locate_sql);
            std::string existing_value = query_result->empty() ? "" : query_result->begin()[1].as<std::string>();
            std::string insert_sql = form_update_sql(ID, interval_by_sec, existing_value);
            DB_invert_index->performQuery(insert_sql);
            dup_guard.push_back(interval_by_sec);
        }
    }
    cout << "Operation done successfully" << endl;
}
