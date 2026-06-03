#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <pqxx/pqxx>
#include <memory>
#include <string>

class DatabaseManager {
private:
    // Static pointer - the only object of the class (Singleton)
    static DatabaseManager* instance;

    // Pointer to connection libpqxx
    std::unique_ptr<pqxx::connection> connection;

    // Noone can create object from outside ("new")
    DatabaseManager() = default;

public:
    // Delete constructor copy and operation =
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    // static method to access Singleton
    static DatabaseManager* getInstance();

    // Methods
    bool connect(const std::string& db_param);
    pqxx::result executeQuery(const std::string& sql);
    void backup();
    bool checkHealth();

    // detructor
    ~DatabaseManager() = default;
};

#endif // DATABASE_MANAGER_H