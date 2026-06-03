#ifndef AUTH_CONTROLLER_H
#define AUTH_CONTROLLER_H

#include <string>
#include <memory>

// Визначаємо SessionToken як аліас для рядка (псевдонім типу)
using SessionToken = std::string;

// Структура для передачі комплексних даних користувача під час реєстрації
struct UserData {
    std::string username;
    std::string password;
    std::string email;
    std::string role; // 'TOURIST', 'AGENCY', 'ADMINISTRATION', 'TECH_SUPPORT'
    std::string first_name;
    std::string second_name;
    std::string phone;
    long long agency_id = 0; // 0 означає відсутність агенції
};

class AuthController {
public:
    // Конструктор за замовчуванням
    AuthController() = default;

    // Публічні методи з твоєї UML-діаграми
    SessionToken login(const std::string& username, const std::string& password);
    bool registerUser(const UserData& userData); // В діаграмі register(userData)
    void logout(const SessionToken& token);
    
    std::string hashPassword(const std::string& password);
    std::string hashEmail(const std::string& email);

    ~AuthController() = default;
};

#endif // AUTH_CONTROLLER_H