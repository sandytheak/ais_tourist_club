# Information System "Travelers' Club"

## Project Overview
This information system is designed to automate the operations of a travel club. The system provides database support for countries, themed tours, and travel agencies, allowing users to select optimal vacation options based on their preferences and scheduled dates.

---

## User Roles and Access

The system distinguishes between four main user groups, each with its own access level and functionality:

1. **Club Administration**
   * Moderation of country lists and tour themes.
   * Oversight of travel agency activities.
   * Generation of analytical reports (popular destinations, visit statistics).
   * Management of the completed tours archive.

2. **Travel Agencies**
   * Adding and updating information about their own tours.
   * Specifying the number of available slots (vacancies) in tours.
   * Managing departure dates and updating prices.
   * Processing applications/requests from tourists.

3. **Tourist (Service User)**
   * Searching and filtering tours by country, theme, and date.
   * Reviewing information regarding agency reliability.
   * Selecting tours and booking spots according to personal preferences.

4. **Technical Staff**
   * Maintaining database integrity (PostgreSQL).
   * Configuring the server-side logic (C++ Backend).
   * Monitoring system performance in Docker containers and managing access permissions.

---

## Functional Capabilities (Reports and Lists)

The system enables the generation of specific lists and tour schedules:

* **Tour Lists:** A directory of tours with available spots (considering booking deadlines).
* **Hot Deals:** Tours with departure dates scheduled in the near future.
* **Travel Archive:** Information on completed tours for preference analysis.
* **Agency Ratings:** A list of partners who have been operating in the market for over 10 years (market veterans).
* **Budget Options:** Filtering tours with costs below the market average.
* **Travel Calendar:** Generating individual schedules for tourists to avoid date overlaps.

---

## Tech Stack
* **Backend:** C++ (Framework: Crow / Drogon)
* **Database:** PostgreSQL
* **Infrastructure:** Docker & Docker Compose
* **Frontend:** HTML5, CSS3, JavaScript (Browser-based)

# Project Structure