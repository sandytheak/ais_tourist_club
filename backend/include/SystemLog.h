#ifndef SYSTEM_LOG_H
#define SYSTEM_LOG_H

#include <string>
#include <vector>
#include "DatabaseManager.h"

// Модель даних сутності SystemLog згідно з UML
struct SystemLog {
    int id;
    std::string timestamp;
    int user_id;
    std::string log_level; // 'INFO', 'WARNING', 'ERROR', 'SECURITY'
    std::string message;

    // Метод з UML-діаграми
    std::string exportToString() const;
};

// Сервісний клас для автоматизації запису логів у БД
class Logger {
private:
    static DatabaseManager* db;

public:
    // Статичні методи для швидкого виклику з будь-якого місця програми
    static void log(int user_id, const std::string& level, const std::string& msg);
    static std::vector<SystemLog> getLogs();
};

#endif // SYSTEM_LOG_H