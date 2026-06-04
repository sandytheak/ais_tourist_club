#include "BookingManager.h"
#include <iostream>
#include <sstream>

BookingManager::BookingManager() {
    db = DatabaseManager::getInstance();
}

// Допоміжний мапінг з рядка БД в об'єкт Booking
Booking mapRowToBooking(const pqxx::row& row) {
    Booking b;
    b.id = row["id"].as<long long>();
    b.tour_id = row["tour_id"].as<long long>();
    b.tourist_id = row["tourist_id"].as<long long>();
    b.booking_date = row["booking_date"].c_str();
    b.status = row["status"].c_str();
    b.total_price = row["total_price"].as<double>();
    return b;
}

bool BookingManager::createBooking(long long tourist_id, long long tour_id) {
    try {
        // 1. Спочатку перевіряємо ціну туру та наявність вільних місць
        std::stringstream checkQuery;
        checkQuery << "SELECT price, total_slots, booking_slots FROM tour WHERE id = " << tour_id << ";";
        pqxx::result res = db->executeQuery(checkQuery.str());

        if (res.empty()) {
            std::cerr << "[BOOKING ERROR] Tour ID " << tour_id << " not found." << std::endl;
            return false;
        }

        double price = res[0]["price"].as<double>();
        int total_slots = res[0]["total_slots"].as<int>();
        int booking_slots = res[0]["booking_slots"].as<int>();

        if (booking_slots >= total_slots) {
            std::cerr << "[BOOKING ERROR] No available slots left for Tour ID " << tour_id << std::endl;
            return false;
        }

        // 2. Вставляємо запис про бронювання зі статусом 'Reserved' та повною вартістю туру (total_price)
        std::stringstream insertQuery;
        insertQuery << "INSERT INTO booking (tour_id, tourist_id, status, total_price) VALUES ("
            << tour_id << ", " << tourist_id << ", 'Reserved', " << price << ") RETURNING id;";
        
        pqxx::result insertRes = db->executeQuery(insertQuery.str());
        long long new_booking_id = insertRes[0]["id"].as<long long>();

        // 3. Інкрементуємо кількість зайнятих місць у турі
        std::stringstream updateTourQuery;
        updateTourQuery << "UPDATE tour SET booking_slots = booking_slots + 1 WHERE id = " << tour_id << ";";
        db->executeQuery(updateTourQuery.str());

        std::cout << "[SUCCESS] Booking ID " << new_booking_id << " created under 'Reserved' status." << std::endl;
        return true;
    } 
    catch (const std::exception& e) {
        std::cerr << "[BOOKING CRITICAL] Transaction failed: " << e.what() << std::endl;
        return false;
    }
}

void BookingManager::confirmPayment(long long booking_id) {
    try {
        // Оновлюємо статус на Paid (згідно з бізнес-флоу системи)
        std::stringstream ss;
        ss << "UPDATE booking SET status = 'Paid' WHERE id = " << booking_id << ";";
        db->executeQuery(ss.str());
        std::cout << "[BOOKING] Payment confirmed for Booking ID: " << booking_id << ". Status set to Paid." << std::endl;
    } 
    catch (const std::exception& e) {
        std::cerr << "[BOOKING ERROR] Confirm payment failed: " << e.what() << std::endl;
    }
}

void BookingManager::cancelBooking(long long booking_id) {
    try {
        // 1. Нам потрібно дізнатися tour_id цього бронювання, щоб повернути місце в тур
        std::stringstream selectQuery;
        selectQuery << "SELECT tour_id, status FROM booking WHERE id = " << booking_id << ";";
        pqxx::result res = db->executeQuery(selectQuery.str());

        if (res.empty()) return;

        long long tour_id = res[0]["tour_id"].as<long long>();
        std::string current_status = res[0]["status"].c_str();

        // Якщо воно вже було скасоване чи архівоване — нічого не робимо
        if (current_status == "Archived") return;

        // 2. Оновлюємо статус бронювання в базі даних
        std::stringstream updateBooking;
        updateBooking << "UPDATE booking SET status = 'Archived' WHERE id = " << booking_id << ";";
        db->executeQuery(updateBooking.str());

        // 3. Повертаємо слот туру назад (декремент кошика місць)
        std::stringstream updateTour;
        updateTour << "UPDATE tour SET booking_slots = GREATEST(0, booking_slots - 1) WHERE id = " << tour_id << ";";
        db->executeQuery(updateTour.str());

        std::cout << "[BOOKING] Booking ID " << booking_id << " cancelled. Slot returned to Tour ID " << tour_id << std::endl;
    } 
    catch (const std::exception& e) {
        std::cerr << "[BOOKING ERROR] Cancel booking failed: " << e.what() << std::endl;
    }
}

std::vector<Booking> BookingManager::getUserBookings(long long tourist_id) {
    std::vector<Booking> list;
    try {
        std::stringstream ss;
        ss << "SELECT * FROM booking WHERE tourist_id = " << tourist_id << ";";
        pqxx::result res = db->executeQuery(ss.str());
        for (const auto& row : res) {
            list.push_back(mapRowToBooking(row));
        }
    } 
    catch (const std::exception& e) {
        std::cerr << "[BOOKING ERROR] Fetching user bookings failed: " << e.what() << std::endl;
    }
    return list;
}