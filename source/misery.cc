#include <iostream>
#include "engine.hh"

int main() {
    try {
        Chassis chassis(800, 600, ColorRGB{0.2f, 0.2f, 0.2f});
        Engine ngn(std::move(chassis));
        // ngn.run_loop();
    } catch (std::runtime_error& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
