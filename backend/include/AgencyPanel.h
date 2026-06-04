#ifndef AGENCY_PANEL_H
#define AGENCY_PANEL_H

#include <string>
#include <vector>
#include "TourManager.h"

class AgencyPanel {
public:
    AgencyPanel() = default;

    // Методи згідно з UML Boundary
    std::string showCreateTourForm(); // Читає базовий HTML
    std::string renderMyTour(const std::vector<Tour>& tours); // Вставляє тури в HTML
    std::string renderBookingRequests();

    ~AgencyPanel() = default;
};

#endif // AGENCY_PANEL_H