#include "motor_command_watchdog.hpp"

MotorCommandWatchdog::MotorCommandWatchdog(
    std::chrono::milliseconds timeout)
    : timeout_(timeout) {
}

void MotorCommandWatchdog::update(
    const MotorCurrentCommand& requested,
    Clock::time_point now) {
    last_command_ = requested;
    last_update_time_ = now;
}

MotorCurrentCommand MotorCommandWatchdog::command_for_send(
    const MotorCommandSafety& safety,
    Clock::time_point now) const {
    if (!last_command_ || !last_update_time_ ||
        now - *last_update_time_ >= timeout_) {
        return {};
    }

    return make_safe_command(*last_command_, safety);
}