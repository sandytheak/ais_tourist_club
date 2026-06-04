#include "DatabaseManager.h"
#include "AuthController.h"
#include "TourManager.h"
#include "BookingManager.h"
#include "AdminControl.h"
#include "SystemLog.h"
#include "Users.h"
#include "TouristDashboard.h"
#include <iostream>
#include "AuthUI.h"
#include "AdminPanel.h"
#include "AgencyPanel.h"
#include "TechMonitor.h"
#include "httplib.h"

int main() {
    std::cout << "=== Starting Tourist Club Web Server ===" << std::endl;

    // 1. Підключення до БД
    DatabaseManager* db = DatabaseManager::getInstance();
    std::string connection_string = "host=127.0.0.1 port=5432 dbname=tourist_club_db user=tourism_app_user password=tourist";
    
    if (!db->connect(connection_string)) {
        std::cerr << "[FATAL] Could not connect to database!" << std::endl;
        return 1;
    }
    std::cout << "[SUCCESS] Database connected." << std::endl;

    // --- ГЕНЕРАЦІЯ ДЕМО-ДАНИХ ---
    try {
        std::cout << "[INIT] Перевірка базових сутностей (Країна та Агенція)..." << std::endl;
        
        // Примусово створюємо країну з ID = 1, якщо її ще немає
        db->executeQuery("INSERT INTO country (id, name, description, visa_required) "
                         "OVERRIDING SYSTEM VALUE VALUES (1, 'Україна', 'Демо країна', FALSE) "
                         "ON CONFLICT (id) DO NOTHING;");
                         
        // Примусово створюємо агенцію з ID = 7, якщо її ще немає
        db->executeQuery("INSERT INTO agencies (id, name, registration_date, expirience_year, is_verified, rate) "
                         "OVERRIDING SYSTEM VALUE VALUES (7, 'GoUkraine Travel', '2026-01-01', 5, TRUE, 5) "
                         "ON CONFLICT (id) DO NOTHING;");
                         
        std::cout << "[INIT] Демо-дані успішно перевірені/створені." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[WARNING] Помилка ініціалізації демо-даних: " << e.what() << std::endl;
    }

    // 2. Ініціалізація менеджерів та інтерфейсів
    TourManager tourManager;
    AuthUI authUI;
    TouristDashboard touristDashboard;
    AgencyPanel agencyPanel;
    AdminPanel adminBoundary;
    AdminControl adminControl;
    TechMonitor techBoundary;
    TechAdmin sysAdmin(0, "root_sec", "root@touristclub.com", "System", "Operator");
    
    AuthController authController;
    // 3. Ініціалізація HTTP сервера
    httplib::Server svr;

    // --- НАЛАШТУВАННЯ МАРШРУТІВ (ROUTES) ---

    // Головна сторінка (редирект на логін)
    svr.Get("/", [](const httplib::Request& req, httplib::Response& res) {
        res.set_redirect("/login");
    });

    // Відображення сторінки авторизації
    svr.Get("/login", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_content(authUI.renderLoginPage(), "text/html");
    });

    // ОБРОБКА ФОРМИ АВТОРИЗАЦІЇ (СПРАВЖНЯ ПЕРЕВІРКА)
    svr.Post("/login", [&](const httplib::Request& req, httplib::Response& res) {
        // Зчитуємо те, що користувач ввів у HTML-форму
        std::string input_username = req.get_param_value("username");
        std::string input_password = req.get_param_value("password");

        std::cout << "[AUTH] Спроба входу від: " << input_username << std::endl;

        try {
            // Перевіряємо користувача в базі даних
            std::string query = "SELECT password_hash, role FROM users WHERE username = '" + input_username + "';";
            pqxx::result dbRes = db->executeQuery(query);

            if (!dbRes.empty()) {
                std::string real_password = dbRes[0]["password_hash"].c_str();
                std::string role = dbRes[0]["role"].c_str();

                // Якщо пароль правильний — перенаправляємо за роллю
                // Хешуємо введений пароль через наш контролер
                std::string hashed_input = authController.hashPassword(input_password);

                // Порівнюємо хеші
                if (hashed_input == real_password) {
                    std::cout << "[AUTH SUCCESS] Доступ дозволено. Роль: " << role << std::endl;
                    
                    if (role == "ADMIN") {
                        res.set_redirect("/admin");
                    } else if (role == "TECH_ADMIN") {
                        res.set_redirect("/tech");
                    } else {
                        res.set_redirect("/dashboard");
                    }
                    return; // Завершуємо виконання, бо логін успішний
                }
            }
            
            // Якщо логін або пароль не підійшли — генеруємо помилку
            std::cout << "[AUTH FAILED] Невірні облікові дані." << std::endl;
            std::string errorAlert = authUI.showError("Невірний логін або пароль!");
            std::string loginPage = authUI.renderLoginPage();
            
            // Хитрий трюк: вклеюємо повідомлення про помилку прямо над формою
            std::string placeholder = "<div style=\"max-width: 400px;";
            size_t pos = loginPage.find(placeholder);
            if (pos != std::string::npos) {
                loginPage.insert(pos, errorAlert);
            }
            res.set_content(loginPage, "text/html");

        } catch (const std::exception& e) {
            res.set_content("Помилка БД: " + std::string(e.what()), "text/plain");
        }
    });

    // --- РОУТИ ДАШБОРДІВ ---

    // 1. Каталог турів (Для Туриста) та його бронювання
    svr.Get("/dashboard", [&](const httplib::Request& req, httplib::Response& res) {
        // Отримуємо всі тури для каталогу
        std::vector<Tour> allTours = tourManager.searchTour(""); 
        std::string html = touristDashboard.renderCatalog(allTours);

        // Отримуємо заброньовані тури користувача taras_guitar
        std::stringstream bookingsHtml;
        try {
            // Використовуємо JOIN, щоб отримати назву туру та статус бронювання
            std::string query = "SELECT t.title, t.price, b.status FROM booking b "
                                "JOIN tour t ON b.tour_id = t.id "
                                "JOIN users u ON b.tourist_id = u.id "
                                "WHERE u.username = 'taras_guitar';";
            pqxx::result bookedRes = db->executeQuery(query);
            
            if (bookedRes.empty()) {
                bookingsHtml << "<p style='color: #7f8c8d;'>У вас ще немає заброньованих турів.</p>";
            } else {
                for (auto row : bookedRes) {
                    bookingsHtml << "<div style='border-left: 4px solid #2980b9; background: #ecf0f1; padding: 10px; margin-bottom: 10px; border-radius: 4px;'>"
                                 << "<strong style='font-size: 16px;'>" << row["title"].c_str() << "</strong><br>"
                                 << "<span style='color: #27ae60;'>Ціна: " << row["price"].c_str() << " UAH</span> | "
                                 << "<span style='color: #e67e22;'>Статус: " << row["status"].c_str() << "</span>"
                                 << "</div>";
                }
            }
        } catch (const std::exception& e) {
            bookingsHtml << "<p style='color: red;'>Помилка завантаження бронювань: " << e.what() << "</p>";
        }

        // Знаходимо мітку і вставляємо згенерований HTML
        std::string placeholder = "@next";
        size_t pos = html.find(placeholder);
        if (pos != std::string::npos) {
            html.replace(pos, placeholder.length(), bookingsHtml.str());
        }

        res.set_content(html, "text/html; charset=utf-8");
    });

    // 2. Панель Адміністратора (Для ADMIN)
    svr.Get("/admin", [&](const httplib::Request& req, httplib::Response& res) {
        std::vector<Agency> verifiedAgencies = adminControl.getVerifiedAgencies();
        double currentSales = adminControl.generateSalesData("ALL");
        std::string currentJsonStats = adminControl.getPopularityStats();
        
        std::string moderationQueueHtml = adminBoundary.renderModerationQueue(verifiedAgencies);
        std::string adminDashboardHtml = adminBoundary.showReportDashboard(currentSales, currentJsonStats, moderationQueueHtml);
        
        res.set_content(adminDashboardHtml, "text/html");
    });

    // 3. Tech Monitor (Для TECH_ADMIN)
    svr.Get("/tech", [&](const httplib::Request& req, httplib::Response& res) {
        std::string serverStatus = sysAdmin.checkServerStatus();
        std::vector<SystemLog> allSystemLogs = sysAdmin.getSystemLogs();
        res.set_content(techBoundary.renderFullDashboard(serverStatus, allSystemLogs), "text/html; charset=utf-8");
    });

    // --- РОУТИ ПАНЕЛІ АГЕНЦІЇ ---

    // Відображення панелі агенції
    svr.Get("/agency", [&](const httplib::Request& req, httplib::Response& res) {
        // Для демо-режиму показуємо тури агенції з ID 7 (як на твоїх скриншотах)
        std::vector<Tour> agencyTours = tourManager.searchTour("AND agency_id = 7");
        res.set_content(agencyPanel.renderMyTour(agencyTours), "text/html");
    });

    // Обробка форми створення туру
    svr.Post("/agency/create_tour", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            // Зчитуємо дані з HTML-форми
            std::string title = req.get_param_value("title");
            std::string price = req.get_param_value("price");
            std::string start_date = req.get_param_value("start_date");
            std::string total_slots = req.get_param_value("total_slots");

            // Виконуємо запит до БД для створення туру ( прив'язуємо до agency_id = 7 та country_id = 1 для демо)
            // Виконуємо запит до БД: виправлено start_date та додано end_date для обходу NOT NULL
            std::string query = "INSERT INTO tour (title, price, start_date, end_date, total_slots, booking_slots, agency_id, country_id) "
                                "VALUES ('" + title + "', " + price + ", '" + start_date + "', '" + start_date + "', " + total_slots + ", 0, 7, 1);";
            
            db->executeQuery(query);
            std::cout << "[AGENCY DASHBOARD] Успішно створено тур: " << title << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Помилка створення туру: " << e.what() << std::endl;
        }
        
        // Перенаправляємо назад на панель, щоб побачити оновлений список
        res.set_redirect("/agency");
    });

    // --- ОБРОБКА БРОНЮВАННЯ ТУРУ ---
    svr.Post("/book_tour", [&](const httplib::Request& req, httplib::Response& res) {
        std::string tour_id_str = req.get_param_value("tour_id");

        try {
            // Для демо-режиму отримуємо ID нашого туриста "taras_guitar"
            pqxx::result userRes = db->executeQuery("SELECT id FROM users WHERE username = 'taras_guitar' LIMIT 1;");
            
            if (!userRes.empty()) {
                std::string tourist_id = userRes[0][0].c_str();

                // 1. Отримуємо ціну туру для запису чека
                pqxx::result tourRes = db->executeQuery("SELECT price FROM tour WHERE id = " + tour_id_str + ";");
                std::string price = "0.00";
                if (!tourRes.empty()) price = tourRes[0][0].c_str();

                // 2. Створюємо запис у таблиці booking
                std::string insertBooking = "INSERT INTO booking (tour_id, tourist_id, status, total_price) "
                                            "VALUES (" + tour_id_str + ", " + tourist_id + ", 'Reserved', " + price + ");";
                db->executeQuery(insertBooking);

                // 3. Збільшуємо лічильник заброньованих місць у самому турі
                std::string updateTour = "UPDATE tour SET booking_slots = booking_slots + 1 WHERE id = " + tour_id_str + ";";
                db->executeQuery(updateTour);
                
                std::cout << "[BOOKING SUCCESS] Тур " << tour_id_str << " заброньовано!" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Помилка бронювання: " << e.what() << std::endl;
        }

        // Одразу перенаправляємо назад у каталог турів
        res.set_redirect("/dashboard");
    });

    // --- ЕКСПОРТ ФІНАНСОВОГО ЗВІТУ ---
    svr.Post("/admin/export", [&](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[ADMIN] Генерація звіту для завантаження..." << std::endl;

        // 1. Отримуємо актуальні дані
        double currentSales = adminControl.generateSalesData("ALL");
        std::string currentJsonStats = adminControl.getPopularityStats();

        // 2. Формуємо красивий текст документа
        std::stringstream report;
        report << "=================================================\n";
        report << "             ФІНАНСОВИЙ ЗВІТ ТУРКЛУБУ            \n";
        report << "=================================================\n\n";
        
        // Додаємо дату (просто як частину тексту)
        report << "Загальний дохід системи: " << std::fixed << std::setprecision(2) << currentSales << " UAH\n\n";
        
        report << "-------------------------------------------------\n";
        report << "СТАТИСТИКА БРОНЮВАНЬ (JSON DUMP):\n";
        report << "-------------------------------------------------\n";
        report << currentJsonStats << "\n\n";
        
        report << "=================================================\n";
        report << "Згенеровано автоматично системою TouristClub.\n";

        // 3. Магія HTTP-заголовків: змушуємо браузер завантажити файл
        res.set_header("Content-Disposition", "attachment; filename=\"financial_report.txt\"");
        
        // 4. Відправляємо контент
        res.set_content(report.str(), "text/plain; charset=utf-8");
    });

    // --- СТВОРЕННЯ СИСТЕМНОГО БЕКАПУ (TECH_ADMIN) ---
    svr.Post("/tech/backup", [&](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[TECH_ADMIN] Ініційовано створення бекапу..." << std::endl;

        // 1. Формуємо команду для ОС (використовуємо pg_dump)
        // Пароль передаємо через змінну оточення PGPASSWORD для безпеки
        // Файл збережеться у форматі: backup_YYYYMMDD_HHMMSS.sql
        std::string command = "PGPASSWORD=tourist pg_dump -h 127.0.0.1 -p 5432 -U tourism_app_user -d tourist_club_db -F c -f backup_$(date +%Y%m%d_%H%M%S).dump";
        
        // Виконуємо системну команду
        int result = system(command.c_str());

        try {
            // 2. Аналізуємо результат і формуємо лог
            std::string statusMsg = (result == 0) ? "Database backup created successfully via pg_dump." : "Failed to create database backup.";
            std::string logLevel = (result == 0) ? "SECURITY" : "ERROR";

            // 3. Записуємо подію в System Log (Системна дія = NULL)
            std::string logQuery = "INSERT INTO system_log (user_id, log_level, message) "
                                   "VALUES (NULL, '" + logLevel + "', '" + statusMsg + "');";
            db->executeQuery(logQuery);
            
            if (result == 0) {
                std::cout << "[SUCCESS] Бекап успішно збережено у директорії з сервером." << std::endl;
            }
            
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Помилка запису логу: " << e.what() << std::endl;
        }

        // 4. Одразу перенаправляємо назад, щоб побачити новий запис у терміналі
        res.set_redirect("/tech");
    });

    // --- ЗАПУСК СЕРВЕРА ---
    std::cout << "\n=========================================" << std::endl;
    std::cout << "🚀 Вебсервер успішно запущено!" << std::endl;
    std::cout << "Відкрийте браузер за адресою: http://localhost:8080" << std::endl;
    std::cout << "Щоб зупинити сервер, натисніть Ctrl+C" << std::endl;
    std::cout << "=========================================\n" << std::endl;

    // Сервер слухає всі мережеві інтерфейси (0.0.0.0) на порту 8080
    svr.listen("0.0.0.0", 8080);

    return 0;
}