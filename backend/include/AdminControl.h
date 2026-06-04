#ifndef ADMIN_CONTROL_H
#define ADMIN_CONTROL_H

#include <string>
#include <vector>
#include "DatabaseManager.h"

// Модель даних Агенції згідно з UML (Page 2)
struct Agency {
    long long id;
    std::string name;
    std::string registration_date;
    int experience_years; // Виправлено описку expirience_year
    bool is_verified;
    int rate;
};

class AdminControl {
private:
    // Поле згідно з UML-діаграмою
    DatabaseManager* db;

public:
    AdminControl();

    // Методи з UML-діаграми (Page 1)
    std::string getPopularityStats(); // Повертає json-рядок
    std::vector<Agency> getVerifiedAgencies(); // Виправлено з getVeteronAgencies
    double generateSalesData(const std::string& period); // Повертає суму продажів
    std::string exportToPDF(const std::string& data_path); // Повертає шлях до файлу
    void resolveConflict(long long booking_id); // Вирішення спірних бронювань

    ~AdminControl() = default;
};

#endif // ADMIN_CONTROL_H