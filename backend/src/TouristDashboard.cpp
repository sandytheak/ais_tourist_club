#include "TouristDashboard.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::string TouristDashboard::renderCatalog(const std::vector<Tour>& tours) {
    std::string filepath = "../../frontend/tourist_dashboard.html";
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        std::cerr << "[Boundary ERROR] Не знайдено файл: " << filepath << std::endl;
        return "<h2 style='color:red;'>Помилка 404: Шаблон каталогу не знайдено</h2>";
    }

    // Читаємо шаблон
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string html = buffer.str();
    file.close();

    // Генеруємо HTML-код для кожного туру з бази
    std::stringstream toursHtml;
    if (tours.empty()) {
        toursHtml << "<p style='text-align:center; color:#888;'>Наразі немає доступних турів.</p>";
    } else {
        for (const auto& tour : tours) {
            int available_slots = tour.total_slots - tour.booking_slots;
            
            toursHtml << "<div style='border: 1px solid #ddd; padding: 20px; border-radius: 8px; margin-bottom: 15px; display: flex; justify-content: space-between; align-items: center;'>\n"
                      << "  <div>\n"
                      << "    <h3 style='margin: 0 0 10px 0; color: #0056b3;'>" << tour.title << "</h3>\n"
                      << "    <p style='margin: 5px 0; color: #555;'>" << tour.description << "</p>\n"
                      << "    <p style='margin: 5px 0;'><strong>Вільних місць:</strong> " << available_slots << " з " << tour.total_slots << "</p>\n"
                      << "  </div>\n"
                      << "  <div style='text-align: right;'>\n"
                      << "    <h3 style='margin: 0 0 10px 0; color: #28a745;'>" << tour.price << " UAH</h3>\n"
                      << "    <form action='/book_tour' method='POST'>\n"
                      << "      <input type='hidden' name='tour_id' value='" << tour.id << "'>\n"
                      << "      <button type='submit' style='background-color: #28a745; color: white; padding: 10px 20px; border: none; border-radius: 4px; cursor: pointer;'>Бронювати</button>\n"
                      << "    </form>\n"
                      << "  </div>\n"
                      << "</div>\n";
        }
    }

    // Замінюємо спеціальну мітку на згенеровані картки
    std::string placeholder = "@tick";
    size_t pos = html.find(placeholder);
    if (pos != std::string::npos) {
        html.replace(pos, placeholder.length(), toursHtml.str());
    }

    return html;
}

std::string TouristDashboard::displayTourDetail(int tour_id) {
    return "<p>Деталі туру ID " + std::to_string(tour_id) + "</p>";
}

void TouristDashboard::processBooking(int tour_id) {
    std::cout << "[Boundary TouristDashboard] Отримано POST-запит на бронювання туру ID: " << tour_id << std::endl;
}