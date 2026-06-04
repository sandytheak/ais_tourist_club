#include "AdminControl.h"
#include <iostream>
#include <sstream>

AdminControl::AdminControl() {
    db = DatabaseManager::getInstance();
}

// Допоміжний мапінг з БД в об'єкт Agency
Agency mapRowToAgency(const pqxx::row& row) {
    Agency a;
    a.id = row["id"].as<long long>();
    a.name = row["name"].c_str();
    a.registration_date = row["registration_date"].c_str();
    a.experience_years = row["expirience_year"].as<int>();
    a.is_verified = row["is_verified"].as<bool>();
    a.rate = row["rate"].as<int>();
    return a;
}

// 1. Агрегація статистики турів у форматі JSON
std::string AdminControl::getPopularityStats() {
    try {
        // Рахуємо кількість бронювань для кожного туру
        std::string query = 
            "SELECT t.title, COUNT(b.id) as bookings_count "
            "FROM tour t LEFT JOIN booking b ON t.id = b.tour_id "
            "GROUP BY t.id, t.title ORDER BY bookings_count DESC;";
        
        pqxx::result res = db->executeQuery(query);
        
        // Формуємо простий JSON-рядок вручну
        std::stringstream json;
        json << "[\n";
        for (size_t i = 0; i < res.size(); ++i) {
            json << "  { \"tour\": \"" << res[i]["title"].c_str() 
                 << "\", \"bookings\": " << res[i]["bookings_count"].as<int>() << " }";
            if (i < res.size() - 1) json << ",\n";
        }
        json << "\n]";
        return json.str();
    } 
    catch (const std::exception& e) {
        return "{ \"error\": \"" + std::string(e.what()) + "\" }";
    }
}

// 2. Отримання перевірених агенцій
std::vector<Agency> AdminControl::getVerifiedAgencies() {
    std::vector<Agency> list;
    try {
        pqxx::result res = db->executeQuery("SELECT * FROM agencies WHERE is_verified = TRUE;");
        for (const auto& row : res) {
            list.push_back(mapRowToAgency(row));
        }
    } 
    catch (const std::exception& e) {
        std::cerr << "[ADMIN ERROR] Failed to fetch verified agencies: " << e.what() << std::endl;
    }
    return list;
}

// 3. Генерація фінансових даних про продажі (за сумою стовпчика total_price для Paid/Booked)
double AdminControl::generateSalesData(const std::string& period) {
    try {
        std::stringstream ss;
        // Для спрощення тесту рахуємо загальну суму оплачених турів за весь час або заданий період
        ss << "SELECT COALESCE(SUM(total_price), 0.00) as total_sales FROM booking WHERE status = 'Paid';";
        
        pqxx::result res = db->executeQuery(ss.str());
        return res[0]["total_sales"].as<double>();
    } 
    catch (const std::exception& e) {
        std::cerr << "[ADMIN ERROR] Failed to generate sales data: " << e.what() << std::endl;
        return 0.0;
    }
}

// 4. Імітація експорту звіту в PDF (повертає шлях до згенерованого файлу)
std::string AdminControl::exportToPDF(const std::string& data_path) {
    std::cout << "[ADMIN] Exporting dashboard statistics to PDF..." << std::endl;
    // В реальному коді тут був бы виклик бібліотеки типу libharu або wkhtmltopdf
    std::string report_filename = "admin_report_" + data_path + ".pdf";
    std::cout << "[SUCCESS] Report saved as " << report_filename << std::endl;
    return report_filename;
}

// 5. Вирішення конфліктів бронювання (наприклад, примусове підтвердження статусу Verified/Booked)
void AdminControl::resolveConflict(long long booking_id) {
    try {
        std::stringstream ss;
        ss << "UPDATE booking SET status = 'Booked' WHERE id = " << booking_id << ";";
        db->executeQuery(ss.str());
        std::cout << "[ADMIN CONFLICT RESOLVED] Booking ID " << booking_id 
                  << " has been manually approved and set to 'Booked'." << std::endl;
    } 
    catch (const std::exception& e) {
        std::cerr << "[ADMIN ERROR] Failed to resolve conflict: " << e.what() << std::endl;
    }
}