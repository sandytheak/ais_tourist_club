#include "AuthUI.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::string AuthUI::renderLoginPage() {
    // Шлях до файлу відносно папки backend/build, звідки ми запускаємо сервер
    std::string filepath = "../../frontend/login.html";
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        std::cerr << "[Boundary ERROR] Не вдалося знайти файл інтерфейсу: " << filepath << std::endl;
        return showError("Помилка завантаження інтерфейсу (404).");
    }

    // Читаємо весь вміст файлу в рядок
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

void AuthUI::handleLoginSubmit(const FormData& request) {
    std::cout << "[Boundary AuthUI] Отримано POST-запит форми авторизації." << std::endl;
    if (request.find("username") != request.end()) {
        std::cout << "[Boundary AuthUI] Передача логіну '" << request.at("username") 
                  << "' до контролера AuthController..." << std::endl;
    }
}

std::string AuthUI::showError(const std::string& msg) {
    std::stringstream html;
    html << "<div style=\"background-color: #ffe6e6; color: #cc0000; padding: 15px;\">\n"
         << "  <strong>Помилка Авторизації:</strong> " << msg << "\n"
         << "</div>";
    return html.str();
}