#include "Users.h"
#include <cstdlib> // Для system()

// --- Базовий клас User ---
User::User(int id, const std::string& uname, const std::string& email, UserRole r)
    : id(id), username(uname), email(email), role(r) {}

int User::getId() const { return id; }
UserRole User::getRole() const { return role; }

bool User::verifyAccess(const std::string& pwd_hash) const {
    return password_hash == pwd_hash; 
}

void User::logout() const {
    std::cout << "[USER] User " << username << " logged out." << std::endl;
}


// --- Клас Tourist ---
Tourist::Tourist(int id, const std::string& uname, const std::string& email, 
                 const std::string& fname, const std::string& lname, const std::string& p)
    : User(id, uname, email, UserRole::TOURIST), first_name(fname), last_name(lname), phone(p) {}

std::vector<Tour> Tourist::searchTours(TourManager& tm, const std::string& filters) {
    return tm.searchTour(filters);
}

bool Tourist::bookTour(BookingManager& bm, int tour_id) {
    return bm.createBooking(this->id, tour_id);
}

bool Tourist::payForTour(BookingManager& bm, int booking_id) {
    bm.confirmPayment(booking_id);
    return true;
}


// --- Клас AgencyManager ---
AgencyManager::AgencyManager(int id, const std::string& uname, const std::string& email, int agency_id)
    : User(id, uname, email, UserRole::AGENCY_MANAGER), agency_id(agency_id) {}

void AgencyManager::updateTour(TourManager& tm, int tour_id) {
    std::cout << "[AGENCY] Updating tour ID " << tour_id << " for agency " << agency_id << std::endl;
    // Логіка оновлення через TourManager
}


// --- Клас Administration ---
Administration::Administration(int id, const std::string& uname, const std::string& email, 
                               const std::string& fname, const std::string& lname)
    : User(id, uname, email, UserRole::ADMIN), first_name(fname), last_name(lname) {}

void Administration::verifyAgency(AdminControl& ac, int agency_id) {
    std::cout << "[ADMIN] Moderation: Verifying agency ID " << agency_id << std::endl;
    // Виклик SQL через AdminControl для оновлення is_verified = TRUE
}

void Administration::resolveDispute(AdminControl& ac, int booking_id) {
    ac.resolveConflict(booking_id);
}

std::string Administration::requestReportGenerator(AdminControl& ac) {
    return ac.getPopularityStats();
}


// --- Клас TechAdmin ---
TechAdmin::TechAdmin(int id, const std::string& uname, const std::string& email, 
                     const std::string& fname, const std::string& lname)
    : User(id, uname, email, UserRole::TECH_ADMIN), first_name(fname), last_name(lname) {}

std::vector<SystemLog> TechAdmin::getSystemLogs() {
    return Logger::getLogs();
}

std::string TechAdmin::checkServerStatus() {
    // В реальності тут був би пінг до БД
    return "Server Status: ONLINE. Database: CONNECTED.";
}

bool TechAdmin::performBackup() {
    std::cout << "[TECH ADMIN] Initiating secure database backup..." << std::endl;
    Logger::log(this->id, "SECURITY", "Database backup initiated manually by TechAdmin.");
    
    // Системний виклик для створення дампу бази даних в Linux
    // (Потребує встановленого клієнта postgresql та налаштованого .pgpass)
    int result = system("docker exec postgres-tour pg_dump -U tourism_app_user tourist_club_db > db_backup.sql");
    
    if (result == 0) {
        std::cout << "[SUCCESS] Backup saved to db_backup.sql" << std::endl;
        return true;
    } else {
        std::cerr << "[ERROR] Backup failed. Check Docker permissions." << std::endl;
        return false;
    }
}

void TechAdmin::resetUserPassword(int target_user_id) {
    std::cout << "[TECH ADMIN] Resetting password for User ID " << target_user_id << std::endl;
    Logger::log(this->id, "WARNING", "Password reset enforced for User ID " + std::to_string(target_user_id));
}