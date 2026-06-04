#ifndef TOURIST_DASHBOARD_H
#define TOURIST_DASHBOARD_H

#include <string>
#include <vector>
#include "TourManager.h" // Підключаємо модель Tour

class TouristDashboard {
public:
    TouristDashboard() = default;

    // Методи згідно з UML Boundary
    std::string renderCatalog(const std::vector<Tour>& tours);
    std::string displayTourDetail(int tour_id);
    void processBooking(int tour_id);

    ~TouristDashboard() = default;
};

#endif // TOURIST_DASHBOARD_H