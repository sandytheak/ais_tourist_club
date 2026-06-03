#include "DatabaseManager.h"
#include <iostream>
#include <cstdlib>

// Initialisation of static instance
DatabaseManager* DatabaseManager::instance = nullptr;

DatabaseManager* DatabaseManager::getInstance() {
    if (instance == nullptr) {
        instance = new DatabaseManager();
    }
    return instance;
}

bool DatabaseManager::connect(const std::string& db_param) {
    try {
        // Замість make_unique ініціалізуємо через оператор new,
        // це змусить компілятор обійти баг сумісності ABI в GCC 13
        connection = std::unique_ptr<pqxx::connection>(new pqxx::connection(db_param));
        
        if (connection && connection->is_open()) {
            std::cout << "[SUCCESS] Connected to PostgreSQL successfully: " 
                      << connection->dbname() << std::endl;
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Connection failed: " << e.what() << std::endl;
    }
    return false;
}

pqxx::result DatabaseManager::executeQuery(const std::string& sql) {
    if (!connection || !connection->is_open()) {
        throw std::runtime_error("Database is not connected!");
    }

    // Create transaction for safe request
    pqxx::work tx(*connection);
    pqxx::result res = tx.exec(sql);
    tx.commit(); // end of transaction

    return res;
}

bool DatabaseManager::checkHealth() {
    try {
        if (connection && connection->is_open()) {
            // crate easy request ot dtb
            pqxx::work tx(*connection);
            tx.exec1("SELECT 1;");
            return true;
        }
    } catch (...) {
        std::cerr << "[WARNING] Database health check failed!" << std::endl;
    }
    return false;
}

void DatabaseManager::backup() {
    std::cout << "[INFO] Starting database backup sequence..." << std::endl;
    
    // Backup via pg_dump
    // Create .sql copy from inside of container
    int result = std::system("docker exec postgres-tour pg_dump -U postgres tourist_club_db > backup.sql");
    
    if (result == 0) {
        std::cout << "[SUCCESS] Backup saved to backup.sql" << std::endl;
    } else {
        std::cerr << "[ERROR] Backup failed! Check if Docker container is running." << std::endl;
    }
}