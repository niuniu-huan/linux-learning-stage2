#include <iomanip>
#include <iostream>

class Motor {
public:
    Motor(int id, double max_temperature_c)
        : id_{id}, max_temperature_c_{max_temperature_c} {}

    void update_feedback(double position_rad,
                         double velocity_rad_s,
                         double temperature_c) {
        position_rad_ = position_rad;
        velocity_rad_s_ = velocity_rad_s;
        temperature_c_ = temperature_c;
    }

    bool enable() {
        if (temperature_c_ > max_temperature_c_) {
            return false;
        }

        enabled_ = true;
        return true;
    }

    void print_status() const {
        std::cout << std::fixed << std::setprecision(2)
                  << "Motor " << id_
                  << " | pos=" << position_rad_ << " rad"
                  << " | vel=" << velocity_rad_s_ << " rad/s"
                  << " | temp=" << temperature_c_ << " C"
                  << " | enabled=" << std::boolalpha << enabled_
                  << '\n';
    }

private:
    int id_;
    double max_temperature_c_;
    double position_rad_{0.0};
    double velocity_rad_s_{0.0};
    double temperature_c_{0.0};
    bool enabled_{false};
};

int main() {
    Motor motor{1, 80.0};

    motor.update_feedback(1.25, 6.28, 36.8);

    if (!motor.enable()) {
        std::cerr << "Motor temperature is too high.\n";
        return 1;
    }

    motor.print_status();
    return 0;
}