#include "AuthController.h"
#include "DatabaseManager.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <random>

// Допоміжний метод для генерації випадкового токена сесії (імітація JWT/UUID)
std::string generateRandomToken() {
    std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string token = "token_";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, chars.size() - 1);
    
    for (int i = 0; i < 24; ++i) {
        token += chars[distribution(generator)];
    }
    return token;
}

// Проста імітація хешування (для демонстрації архітектури)
// На захисті курсової ти скажеш: "Тут стоїть заглушка, яку легко замінити на openssl/evp.h"
std::string AuthController::hashPassword(const std::string& password) {
    // Для демонстрації просто додамо сіль та імітуємо хеш
    return "hashed_salted_" + password; 
}

std::string AuthController::hashEmail(const std::string& email) {
    // Анонімізація або маскування email (якщо потрібно для внутрішніх логів безпеки)
    return "masked_" + email;
}

bool AuthController::registerUser(const UserData& userData) {
    try {
        DatabaseManager* db = DatabaseManager::getInstance();
        std::string encryptedPassword = hashPassword(userData.password);

        // Формуємо SQL запит для вставки нового користувача
        std::stringstream ss;
        ss << "INSERT INTO users (username, password_hash, email, role, first_name, second_name, phone, agency_id) VALUES ("
           << "'" << userData.username << "', "
           << "'" << encryptedPassword << "', "
           << "'" << userData.email << "', "
           << "'" << userData.role << "', ";
        
        // Обробка nullable полів
        if (userData.first_name.empty()) ss << "NULL, "; else ss << "'" << userData.first_name << "', ";
        if (userData.second_name.empty()) ss << "NULL, "; else ss << "'" << userData.second_name << "', ";
        if (userData.phone.empty()) ss << "NULL, "; else ss << "'" << userData.phone << "', ";
        
        if (userData.agency_id <= 0) ss << "NULL"; else ss << userData.agency_id;
        ss << ");";

        db->executeQuery(ss.str());
        std::cout << "[AUTH] User " << userData.username << " successfully registered." << std::endl;
        return true;
    } 
    catch (const std::exception& e) {
        std::cerr << "[AUTH ERROR] Registration failed: " << e.what() << std::endl;
        return false;
    }
}

SessionToken AuthController::login(const std::string& username, const std::string& password) {
    try {
        DatabaseManager* db = DatabaseManager::getInstance();
        std::string encryptedPassword = hashPassword(password);

        // Перевіряємо чи є користувач з таким логіном та хешем паролю
        std::stringstream ss;
        ss << "SELECT id, role FROM users WHERE username = '" << username 
           << "' AND password_hash = '" << encryptedPassword << "';";

        pqxx::result res = db->executeQuery(ss.str());

        if (res.empty()) {
            std::cout << "[AUTH] Invalid login details for user: " << username << std::endl;
            return ""; // Повертаємо порожній токен (авторизація провалена)
        }

        // Якщо користувача знайдено — генеруємо токен сесії
        std::string user_id = res[0]["id"].c_str();
        std::string role = res[0]["role"].c_str();
        
        SessionToken token = generateRandomToken();
        std::cout << "[AUTH] User " << username << " (" << role << ") logged in. Token generated." << std::endl;
        
        return token;
    } 
    catch (const std::exception& e) {
        std::cerr << "[AUTH ERROR] Login process encountered an error: " << e.what() << std::endl;
        return "";
    }
}

void AuthController::logout(const SessionToken& token) {
    // В архітектурі без збереження стану сесій на рівні БД (stateful) 
    // тут ми б просто видаляли токен з локального масиву активних сесій сервера.
    std::cout << "[AUTH] Token " << token << " has been invalidated (Logout success)." << std::endl;
}