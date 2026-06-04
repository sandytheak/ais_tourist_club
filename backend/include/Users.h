#ifndef USERS_H
#define USERS_H

#include <string>
#include <vector>
#include <iostream>
#include "TourManager.h"
#include "BookingManager.h"
#include "AdminControl.h"
#include "SystemLog.h"

// Перелічення для ролей
enum class UserRole { TOURIST, AGENCY_MANAGER, ADMIN, TECH_ADMIN };

// Базовий клас Entity User
class User {
protected:
    int id;
    std::string username;
    std::string password_hash;
    std::string email;
    UserRole role;

public:
    User(int id, const std::string& uname, const std::string& email, UserRole r);
    virtual ~User() = default;

    int getId() const;
    UserRole getRole() const;
    bool verifyAccess(const std::string& pwd_hash) const;
    void logout() const;
};

// Спадкоємець 1: Tourist
class Tourist : public User {
private:
    std::string first_name;
    std::string last_name;
    std::string phone;

public:
    Tourist(int id, const std::string& uname, const std::string& email, 
            const std::string& fname, const std::string& lname, const std::string& phone);

    std::vector<Tour> searchTours(TourManager& tm, const std::string& filters);
    bool bookTour(BookingManager& bm, int tour_id);
    bool payForTour(BookingManager& bm, int booking_id);
};

// Спадкоємець 2: AgencyManager
class AgencyManager : public User {
private:
    int agency_id;

public:
    AgencyManager(int id, const std::string& uname, const std::string& email, int agency_id);
    void updateTour(TourManager& tm, int tour_id); // Спрощена сигнатура
};

// Спадкоємець 3: Administration
class Administration : public User {
private:
    std::string first_name;
    std::string last_name;

public:
    Administration(int id, const std::string& uname, const std::string& email, 
                   const std::string& fname, const std::string& lname);

    void verifyAgency(AdminControl& ac, int agency_id);
    void resolveDispute(AdminControl& ac, int booking_id);
    std::string requestReportGenerator(AdminControl& ac);
};

// Спадкоємець 4: TechAdmin
class TechAdmin : public User {
private:
    std::string first_name;
    std::string last_name;

public:
    TechAdmin(int id, const std::string& uname, const std::string& email, 
              const std::string& fname, const std::string& lname);

    std::vector<SystemLog> getSystemLogs();
    std::string checkServerStatus();
    bool performBackup();
    void resetUserPassword(int target_user_id);
};

#endif // USERS_H