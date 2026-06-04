#ifndef ADMIN_PANEL_H
#define ADMIN_PANEL_H

#include <string>
#include <vector>
#include "AdminControl.h" // Нам потрібна модель Agency з цього файлу

class AdminPanel {
public:
    AdminPanel() = default;

    // Згідно з UML-діаграмою
    std::string renderModerationQueue(const std::vector<Agency>& agencies);
    std::string showReportDashboard(double totalSales, const std::string& jsonStats, const std::string& moderationHtml);
    std::string triggerExport(const std::string& format);

    ~AdminPanel() = default;
};

#endif // ADMIN_PANEL_H