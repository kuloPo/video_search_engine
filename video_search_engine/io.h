#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <regex>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <pqxx/pqxx>

#include "common.h"
#include "picosha2.h"

class DB_Connector {
public:
    DB_Connector(const std::string& u,
        const std::string& h,
        const std::string& pswd,
        const std::string& db,
        const std::string& p)
        :user{ "user=" + u }, host{ " host=" + h }, password{ " password=" + pswd }, dbname{ " dbname=" + db }, port{ " port=" + p }
    {
        initConnection(conn, user, host, password, dbname, port);
    }

    ~DB_Connector() {
        conn->close();
    }

    void initConnection(std::unique_ptr<pqxx::connection>& c,
        const std::string& u,
        const std::string& h,
        const std::string& pswd,
        const std::string& db,
        const std::string& port)
    {
        try {
            conn = std::make_unique<pqxx::connection>(user + host + password + dbname + port);
            std::cout << "Connected to " << conn->dbname() << '\n';
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            exit(1);
        }
    }

    std::unique_ptr<pqxx::result>& performQuery(const std::string& query)
    {
        try {
            trans = std::make_unique<pqxx::work>(*conn, "trans");
            res = std::make_unique<pqxx::result>(trans->exec(query));
            trans->commit();
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            exit(1);
        }

        return res;
    }
private:
    std::unique_ptr<pqxx::connection> conn;
    std::unique_ptr<pqxx::work> trans;
    std::unique_ptr<pqxx::result> res;

    std::string user;
    std::string host;
    std::string password;
    std::string dbname;
    std::string port;
};

void show_image(const std::vector<Key_Frame*>& key_frames);
std::string write_interval(const std::vector<int>& interval, const std::filesystem::path& filename);
void write_key_frame(const std::vector<Key_Frame*>& key_frames, const std::filesystem::path& path, const std::filesystem::path& filename);
int get_fps(const std::filesystem::path& filename);
std::unique_ptr<DB_Connector> init_db();
