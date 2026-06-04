#include "SystemLog.h"
#include <iostream>
#include <sstream>

// Реалізація методу сутності з UML
std::string SystemLog::exportToString() const {
    std::stringstream ss;
    ss << "[" << timestamp << "] [" << log_level << "] (User ID: " 
       << user_id << ") -> " << message;
    return ss.str();
}

// Ініціалізація статичного менеджера бази
DatabaseManager* Logger::db = DatabaseManager::getInstance();

// Запис логу в PostgreSQL (таблиця system_log)
void Logger::log(int user_id, const std::string& level, const std::string& msg) {
    try {
        std::stringstream ss;
        ss << "INSERT INTO system_log (user_id, log_level, message) VALUES (";
        
        if (user_id <= 0) ss << "NULL, "; else ss << user_id << ", ";
        
        ss << "'" << level << "', '" << msg << "');";
        
        db->executeQuery(ss.str());
        std::cout << "[SYSTEM LOG FIRED] " << level << ": " << msg << std::endl;
    } 
    catch (const std::exception& e) {
        std::cerr << "[LOGGER ERROR] Failed to write system log: " << e.what() << std::endl;
    }
}

// Читання логів для перевірки
std::vector<SystemLog> Logger::getLogs() {
    std::vector<SystemLog> list;
    try {
        pqxx::result res = db->executeQuery("SELECT * FROM system_log ORDER BY id DESC;");
        for (const auto& row : res) {
            SystemLog l;
            l.id = row["id"].as<int>();
            l.timestamp = row["date_time"].c_str();
            l.user_id = row["user_id"].is_null() ? 0 : row["user_id"].as<int>();
            l.log_level = row["log_level"].c_str();
            l.message = row["message"].c_str();
            list.push_back(l);
        }
    } 
    catch (const std::exception& e) {
        std::cerr << "[LOGGER ERROR] Failed to fetch logs: " << e.what() << std::endl;
    }
    return list;
}