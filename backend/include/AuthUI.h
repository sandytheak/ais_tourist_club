#ifndef AUTH_UI_H
#define AUTH_UI_H

#include <string>
#include <map>

// Симуляція структури FormData з UML-діаграми
using FormData = std::map<std::string, std::string>;

class AuthUI {
private:
    FormData login_form;
    FormData registrationForm;

public:
    AuthUI() = default;

    // Методи згідно з Boundary AuthUI
    std::string renderLoginPage();
    void handleLoginSubmit(const FormData& request);
    std::string showError(const std::string& msg);

    ~AuthUI() = default;
};

#endif // AUTH_UI_H