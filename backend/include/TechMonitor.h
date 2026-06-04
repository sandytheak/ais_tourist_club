#ifndef TECH_MONITOR_H
#define TECH_MONITOR_H

#include <string>
#include <vector>
#include "SystemLog.h"

class TechMonitor {
public:
    TechMonitor() = default;

    // Методи згідно з UML Boundary
    std::string renderLogView(const std::vector<SystemLog>& logs);
    std::string displayHealthStatus(const std::string& status);
    std::string showBackUpControl();

    // Допоміжний метод для збірки сторінки
    std::string renderFullDashboard(const std::string& status, const std::vector<SystemLog>& logs);

    ~TechMonitor() = default;
};

#endif // TECH_MONITOR_H