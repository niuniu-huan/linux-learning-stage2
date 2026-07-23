#pragma once

#include "motor_command.hpp"

#include <chrono>
#include <optional>

class MotorCommandWatchdog {
public:
    using Clock = std::chrono::steady_clock;

    explicit MotorCommandWatchdog(std::chrono::milliseconds timeout);

    void update(const MotorCurrentCommand& requested,
                Clock::time_point now);

    MotorCurrentCommand command_for_send(
        const MotorCommandSafety& safety,
        Clock::time_point now) const;

private:
    std::chrono::milliseconds timeout_;
    std::optional<MotorCurrentCommand> last_command_;
    std::optional<Clock::time_point> last_update_time_;
};