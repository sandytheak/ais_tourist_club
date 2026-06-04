#ifndef BOOKING_MANAGER_H
#define BOOKING_MANAGER_H

#include <string>
#include <vector>
#include "DatabaseManager.h"

// Модель даних Бронювання згідно з UML
struct Booking {
    long long id;
    long long tour_id;
    long long tourist_id;
    std::string booking_date;
    std::string status; // 'Reserved', 'Verified', 'Paid', 'Booked', 'Archived'
    double total_price;
};

class BookingManager {
private:
    DatabaseManager* db;

public:
    BookingManager();

    // Основна бізнес-логика для роботи з сутністю Booking
    bool createBooking(long long tourist_id, long long tour_id);
    void confirmPayment(long long booking_id); // Згідно з UML
    void cancelBooking(long long booking_id);  // Згідно з UML
    
    std::vector<Booking> getUserBookings(long long tourist_id);

    ~BookingManager() = default;
};

#endif // BOOKING_MANAGER_H