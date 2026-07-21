#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

struct MotorFeedback {
    int id;
    double position_rad;
    double velocity_rad_s;
    double temperature_c;
};

std::optional<MotorFeedback> parse_motor_feedback(const std::string& frame) {
    std::istringstream stream(frame);

    std::string type;
    std::string id_text;
    std::string position_text;
    std::string velocity_text;
    std::string temperature_text;

    if (!std::getline(stream, type, ',') ||
        !std::getline(stream, id_text, ',') ||
        !std::getline(stream, position_text, ',') ||
        !std::getline(stream, velocity_text, ',') ||
        !std::getline(stream, temperature_text)) {
        return std::nullopt;
    }

    if (type != "MOTOR") {
        return std::nullopt;
    }

    try {
        return MotorFeedback{
            std::stoi(id_text),
            std::stod(position_text),
            std::stod(velocity_text),
            std::stod(temperature_text)
        };
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

int main() {
    const std::string frame = "MOTOR,1,1.25,6.28,36.8";

    const auto feedback = parse_motor_feedback(frame);
    if (!feedback) {
        std::cerr << "Invalid motor feedback frame.\n";
        return 1;
    }

    std::cout << std::fixed << std::setprecision(2)
              << "Motor " << feedback->id
              << " | pos=" << feedback->position_rad << " rad"
              << " | vel=" << feedback->velocity_rad_s << " rad/s"
              << " | temp=" << feedback->temperature_c << " C\n";

    return 0;
}