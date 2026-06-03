#ifndef TOUR_MANAGER_H
#define TOUR_MANAGER_H

#include <string>
#include <vector>
#include "DatabaseManager.h"

// Модель даних Туру (Entity для мапінгу з БД)
struct Tour {
    long long id;
    long long country_id;
    long long agency_id;
    std::string title;
    double price;
    std::string start_date;
    std::string end_date;
    int total_slots;
    int booking_slots;
    std::string theme;
    std::string description;
};

// Структура для створення нового туру
struct TourData {
    long long country_id;
    long long agency_id;
    std::string title;
    double price;
    std::string start_date;
    std::string end_date;
    int total_slots;
    std::string theme;
    std::string description;
};

class TourManager {
private:
    // Поле згідно з UML-діаграмою
    DatabaseManager* db;

public:
    // Конструктор ініціалізує вказівник на Singleton бази даних
    TourManager();

    // Методи з UML-діаграми
    std::vector<Tour> searchTour(const std::string& filters); // Фікс описки з serchTour
    bool checkAvailability(int tour_id);
    std::vector<Tour> getHotTour();
    bool createTour(const TourData& data);
    void applyDiscount(int tour_id, int discount);

    ~TourManager() = default;
};

#endif // TOUR_MANAGER_H