#include "TourManager.h"
#include <iostream>
#include <sstream>

TourManager::TourManager() {
    db = DatabaseManager::getInstance();
}

// Допоміжний метод для конвертації рядка з БД в об'єкт Tour
Tour mapRowToTour(const pqxx::row& row) {
    Tour t;
    t.id = row["id"].as<long long>();
    t.country_id = row["country_id"].as<long long>();
    t.agency_id = row["agency_id"].as<long long>();
    t.title = row["title"].c_str();
    t.price = row["price"].as<double>();
    t.start_date = row["start_date"].c_str();
    t.end_date = row["end_date"].c_str();
    t.total_slots = row["total_slots"].as<int>();
    t.booking_slots = row["booking_slots"].as<int>();
    t.theme = row["theme"].is_null() ? "" : row["theme"].c_str();
    t.description = row["description"].is_null() ? "" : row["description"].c_str();
    return t;
}

bool TourManager::createTour(const TourData& data) {
    try {
        std::stringstream ss;
        ss << "INSERT INTO tour (country_id, agency_id, title, price, start_date, end_date, total_slots, theme, description) VALUES ("
           << data.country_id << ", "
           << data.agency_id << ", "
           << "'" << data.title << "', "
           << data.price << ", "
           << "'" << data.start_date << "', "
           << "'" << data.end_date << "', "
           << data.total_slots << ", "
           << "'" << data.theme << "', "
           << "'" << data.description << "');";

        db->executeQuery(ss.str());
        std::cout << "[TOUR] Tour '" << data.title << "' successfully created." << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[TOUR ERROR] Failed to create tour: " << e.what() << std::endl;
        return false;
    }
}

std::vector<Tour> TourManager::searchTour(const std::string& filters) {
    std::vector<Tour> tours;
    try {
        std::stringstream ss;
        ss << "SELECT * FROM tour WHERE 1=1 " << filters << ";";

        pqxx::result res = db->executeQuery(ss.str());
        for (const auto& row : res) {
            tours.push_back(mapRowToTour(row));
        }
    } catch (const std::exception& e) {
        std::cerr << "[TOUR ERROR] Search failed: " << e.what() << std::endl;
    }
    return tours;
}

bool TourManager::checkAvailability(int tour_id) {
    try {
        std::stringstream ss;
        ss << "SELECT total_slots, booking_slots FROM tour WHERE id = " << tour_id << ";";
        pqxx::result res = db->executeQuery(ss.str());

        if (res.empty()) return false;

        int total = res[0]["total_slots"].as<int>();
        int booked = res[0]["booking_slots"].as<int>();

        return (total - booked) > 0; // Якщо є вільні місця
    } catch (...) {
        return false;
    }
}

std::vector<Tour> TourManager::getHotTour() {
    std::vector<Tour> tours;
    try {
        // "Гарячі тури" — наприклад, де заповнено більше 80% місць, але ще є вільні, 
        // або які починаються дуже скоро. Зробимо вибірку турів із заповненістю.
        std::string query = "SELECT * FROM tour WHERE booking_slots >= (total_slots * 0.8) AND booking_slots < total_slots LIMIT 3;";
        
        pqxx::result res = db->executeQuery(query);
        for (const auto& row : res) {
            tours.push_back(mapRowToTour(row));
        }
    } catch (const std::exception& e) {
        std::cerr << "[TOUR ERROR] Failed to get hot tours: " << e.what() << std::endl;
    }
    return tours;
}

void TourManager::applyDiscount(int tour_id, int discount) {
    try {
        // Розраховуємо коефіцієнт знижки (наприклад, 15% знижки -> помножити ціну на 0.85)
        double multiplier = 1.0 - (discount / 100.0);
        
        std::stringstream ss;
        ss << "UPDATE tour SET price = price * " << multiplier << " WHERE id = " << tour_id << ";";
        
        db->executeQuery(ss.str());
        std::cout << "[TOUR] Applied " << discount << "% discount to tour ID: " << tour_id << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[TOUR ERROR] Failed to apply discount: " << e.what() << std::endl;
    }
}