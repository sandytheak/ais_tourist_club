#include "TechMonitor.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::string TechMonitor::renderLogView(const std::vector<SystemLog>& logs) {
    std::stringstream html;
    for (const auto& log : logs) {
        std::string color = "#c5c8c6"; // Сірий за замовчуванням
        if (log.log_level == "ERROR") color = "#cc6666"; // Червоний
        else if (log.log_level == "SECURITY") color = "#de935f"; // Помаранчевий
        else if (log.log_level == "WARNING") color = "#f0c674"; // Жовтий
        else if (log.log_level == "INFO") color = "#81a2be"; // Синій

        html << "<div style='color: " << color << "; margin-bottom: 8px; border-bottom: 1px dashed #373b41; padding-bottom: 4px;'>"
             << log.exportToString() 
             << "</div>\n";
    }
    return html.str();
}

std::string TechMonitor::displayHealthStatus(const std::string& status) {
    return "<p style='font-size: 16px; font-weight: bold;'>" + status + "</p>";
}

std::string TechMonitor::showBackUpControl() {
    return "<form action='/tech/backup' method='POST'>"
           "<button type='submit' style='background: #81a2be; color: #1d1f21; padding: 10px 20px; border: none; font-weight: bold; cursor: pointer;'>Створити системний Backup БД</button>"
           "</form>";
}

std::string TechMonitor::renderFullDashboard(const std::string& status, const std::vector<SystemLog>& logs) {
    std::string filepath = "../../frontend/tech_monitor.html";
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        std::cerr << "[Boundary ERROR] Не знайдено файл: " << filepath << std::endl;
        return "<h2>Помилка 404: Шаблон TechMonitor не знайдено</h2>";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string html = buffer.str();
    file.close();

    // Генеруємо компоненти
    std::string logsHtml = renderLogView(logs);
    std::string healthHtml = displayHealthStatus(status);
    std::string backupHtml = showBackUpControl();

    // Підставляємо в шаблон
    std::string placeholder = "@1";
    size_t pos = html.find(placeholder);
    if (pos != std::string::npos) html.replace(pos, placeholder.length(), healthHtml);

    placeholder = "@2";
    pos = html.find(placeholder);
    if (pos != std::string::npos) html.replace(pos, placeholder.length(), backupHtml);

    placeholder = "@3";
    pos = html.find(placeholder);
    if (pos != std::string::npos) html.replace(pos, placeholder.length(), logsHtml);

    return html;
}