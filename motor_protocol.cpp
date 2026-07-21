#include "motor_protocol.hpp"

#include <sstream>

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