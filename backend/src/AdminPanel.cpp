#include "AdminPanel.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Генеруємо HTML-блок зі списком агенцій
std::string AdminPanel::renderModerationQueue(const std::vector<Agency>& agencies) {
    std::stringstream html;
    if (agencies.empty()) {
        html << "<p style='color: #7f8c8d;'>Черга порожня. Усі агенції перевірено.</p>";
    } else {
        for (const auto& a : agencies) {
            std::string status = a.is_verified ? "<span style='color:green;'>Перевірено</span>" : "<span style='color:red;'>Очікує</span>";
            html << "<div style='border: 1px solid #bdc3c7; padding: 10px; margin-bottom: 10px; border-radius: 4px;'>\n"
                 << "  <h4 style='margin: 0 0 5px 0;'>" << a.name << "</h4>\n"
                 << "  <p style='margin: 0; font-size: 14px;'>Досвід: " << a.experience_years << " років | Статус: " << status << "</p>\n"
                 << "</div>\n";
        }
    }
    return html.str();
}

// Завантажуємо базовий шаблон і вставляємо всі віджети
std::string AdminPanel::showReportDashboard(double totalSales, const std::string& jsonStats, const std::string& moderationHtml) {
    std::string filepath = "../../frontend/admin_panel.html";
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        std::cerr << "[Boundary ERROR] Не знайдено файл: " << filepath << std::endl;
        return "<h2>Помилка 404: Шаблон адмін-панелі не знайдено</h2>";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string html = buffer.str();
    file.close();

    // Підставляємо чергу модерації
    std::string placeholder = "@1";
    size_t pos = html.find(placeholder);
    if (pos != std::string::npos) html.replace(pos, placeholder.length(), moderationHtml);

    // Підставляємо фінанси
    placeholder = "@2";
    pos = html.find(placeholder);
    // Форматуємо число до 2 знаків після коми
    std::ostringstream streamObj;
    streamObj << std::fixed << std::setprecision(2) << totalSales;
    std::string formattedSales = streamObj.str();

    // Робимо заміну
    if (pos != std::string::npos) {
        html.replace(pos, placeholder.length(), formattedSales + " UAH");
    }

    // Підставляємо JSON
    placeholder = "@3";
    pos = html.find(placeholder);
    if (pos != std::string::npos) html.replace(pos, placeholder.length(), jsonStats);

    return html;
}

std::string AdminPanel::triggerExport(const std::string& format) {
    std::cout << "[Boundary AdminPanel] Запущено експорт дашборду у форматі: " << format << std::endl;
    return "export_triggered";
}