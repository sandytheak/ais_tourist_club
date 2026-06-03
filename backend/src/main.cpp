#include "DatabaseManager.h"
#include "AuthController.h"
#include "TourManager.h"
#include <iostream>

int main() {
    std::cout << "=== Starting Tourist Club Server ===" << std::endl;

    DatabaseManager* db = DatabaseManager::getInstance();
    std::string connection_string = 
        "host=127.0.0.1 port=5432 dbname=tourist_club_db user=tourism_app_user password=tourist";

    if (!db->connect(connection_string)) {
        std::cerr << "[MAIN] Critical Error: Could not connect to database." << std::endl;
        return 1;
    }

    std::cout << "\n=== Preparing Test Infrastructure ===" << std::endl;
    try {
        // Очищаємо старі тести для можливості повторного запуску
        // Замість TRUNCATE пишемо:
        db->executeQuery("DELETE FROM booking; DELETE FROM tour; DELETE FROM country; DELETE FROM agencies;");

        // 1. Створюємо тестову агенцію (id згенерується як 1)
        db->executeQuery(
            "INSERT INTO agencies (name, registration_date, expirience_year, is_verified, rate) "
            "VALUES ('GoUkraine Travel', '2026-01-15', 5, TRUE, 5);"
        );
        std::cout << "[MAIN] Test Agency inserted (ID: 1)" << std::endl;

        // 2. Створюємо тестову країну (id згенерується як 1)
        db->executeQuery(
            "INSERT INTO country (name, description, visa_required) "
            "VALUES ('Україна', 'Неймовірні Карпати та історичний Київ', FALSE);"
        );
        std::cout << "[MAIN] Test Country inserted (ID: 1)" << std::endl;
    } 
    catch (const std::exception& e) {
        std::cerr << "[MAIN ERROR] Infrastructure prep failed: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "\n=== Testing TourManager Flow ===" << std::endl;
    TourManager tourManager;

    // 3. Створюємо структуру для нового туру
    TourData activeTour;
    activeTour.country_id = 1; // Посилаємось на створену країну
    activeTour.agency_id = 1;  // Посилаємось на створену агенцію
    activeTour.title = "Вікенд у Ворохті";
    activeTour.price = 4500.00;
    activeTour.start_date = "2026-07-10";
    activeTour.end_date = "2026-07-13";
    activeTour.total_slots = 10;
    activeTour.theme = "Active Rock & Mountains";
    activeTour.description = "Похід на Чорногірський хребет, вечірній костер та рок-гітара.";

    // 4. Додаємо тур в базу через менеджер
    if (tourManager.createTour(activeTour)) {
        std::cout << "[MAIN] Tour creation test: SUCCESS!" << std::endl;
    } else {
        std::cerr << "[MAIN] Tour creation test: FAILED!" << std::endl;
    }

    // 5. Перевіряємо наявність вільних місць (має повернути true, бо заброньовано 0 з 10)
    std::cout << "\n[MAIN] Checking availability for Tour ID 1..." << std::endl;
    if (tourManager.checkAvailability(1)) {
        std::cout << "[MAIN] Availability check: PASSED! (Slots are free)" << std::endl;
    } else {
        std::cout << "[MAIN] Availability check: FAILED! (No slots)" << std::endl;
    }

    // 6. Тестуємо застосування знижки (наприклад, гаряча знижка 10%)
    std::cout << "\n[MAIN] Applying 10% discount to Tour ID 1..." << std::endl;
    tourManager.applyDiscount(1, 10);

    // 7. Перевіряємо оновлену ціну через пошук туру
    std::cout << "\n[MAIN] Verification via search..." << std::endl;
    std::vector<Tour> searchResults = tourManager.searchTour("AND price < 4200");
    
    if (!searchResults.empty()) {
        std::cout << "[MAIN] Search test: SUCCESS!" << std::endl;
        std::cout << "[FOUND] Title: " << searchResults[0].title << std::endl;
        std::cout << "[FOUND] New Price (after 10% discount): " << searchResults[0].price << " UAH" << std::endl;
    } else {
        std::cerr << "[MAIN] Search test: FAILED! Tour price was not updated." << std::endl;
    }

    std::cout << "\n=== Server tasks completed successfully ===" << std::endl;
    return 0;
}