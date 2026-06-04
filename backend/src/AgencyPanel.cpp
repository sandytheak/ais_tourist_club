#include "AgencyPanel.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::string AgencyPanel::showCreateTourForm() {
    std::string filepath = "../../frontend/agency_panel.html";
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        std::cerr << "[Boundary ERROR] Не знайдено файл: " << filepath << std::endl;
        return "<h2 style='color:red;'>Помилка 404: Шаблон панелі агенції не знайдено</h2>";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string AgencyPanel::renderMyTour(const std::vector<Tour>& tours) {
    // Отримуємо базовий HTML з формою
    std::string html = showCreateTourForm();

    // Генеруємо HTML-код для кожного туру агенції
    std::stringstream toursHtml;
    if (tours.empty()) {
        toursHtml << "<p style='color:#888;'>У вас ще немає створених турів.</p>";
    } else {
        for (const auto& tour : tours) {
            toursHtml << "<div style='border-left: 4px solid #0056b3; background: #f9f9f9; padding: 15px; margin-bottom: 10px;'>\n"
                      << "  <h4 style='margin: 0 0 5px 0;'>" << tour.title << "</h4>\n"
                      << "  <p style='margin: 0; font-size: 14px; color: #555;'>Ціна: <strong>" << tour.price 
                      << " UAH</strong> | Всього місць: <strong>" << tour.total_slots 
                      << "</strong> | Заброньовано: <strong>" << tour.booking_slots << "</strong></p>\n"
                      << "</div>\n";
        }
    }

    // Замінюємо спеціальну мітку
    std::string placeholder = "@";
    size_t pos = html.find(placeholder);
    if (pos != std::string::npos) {
        html.replace(pos, placeholder.length(), toursHtml.str());
    }


    return html;
}

std::string AgencyPanel::renderBookingRequests() {
    std::cout << "[Boundary AgencyPanel] Формування списку заявок на бронювання..." << std::endl;
    return "<p>Список заявок в розробці...</p>";
}