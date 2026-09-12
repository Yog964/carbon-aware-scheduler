#pragma once

namespace carbongrid {

/**
 * @brief Simulation clock to drive discrete event simulation engine.
 */
class SimulationClock {
private:
    double current_time = 0.0;
    double speed_multiplier = 1.0;
    bool is_running = false;

public:
    SimulationClock() = default;

    void start() {
        is_running = true;
    }

    void stop() {
        is_running = false;
    }

    void reset() {
        current_time = 0.0;
        is_running = false;
        speed_multiplier = 1.0;
    }

    void advance(double delta) {
        if (is_running && delta > 0) {
            current_time += delta;
        }
    }

    void set_speed(double speed) {
        if (speed > 0) {
            speed_multiplier = speed;
        }
    }

    double now() const {
        return current_time;
    }
    
    bool running() const {
        return is_running;
    }
    
    double speed() const {
        return speed_multiplier;
    }
};

} // namespace carbongrid
