#pragma once

#include <functional>
#include <vector>

namespace carbongrid {

struct LoadProfile {
    double start_time;
    double tasks_per_sec;
};

class LoadController {
public:
    LoadController() = default;
    
    // Set a fixed rate
    void set_rate(double tasks_per_sec) {
        fixed_rate_ = tasks_per_sec;
        use_profile_ = false;
    }
    
    double get_rate() const {
        return fixed_rate_;
    }
    
    // Ramp profiles: specify rate changes over time
    void set_profile(const std::vector<LoadProfile>& profile) {
        profile_ = profile;
        use_profile_ = true;
    }
    
    // Get rate at a given simulation time (follows the profile)
    double get_rate_at(double time) const {
        if (!use_profile_ || profile_.empty()) {
            return fixed_rate_;
        }
        
        double current_rate = profile_.front().tasks_per_sec;
        for (const auto& entry : profile_) {
            if (entry.start_time <= time) {
                current_rate = entry.tasks_per_sec;
            } else {
                break;
            }
        }
        return current_rate;
    }
    
    // Predefined stress test profiles
    static std::vector<LoadProfile> gentle_ramp() {
        return {{0.0, 10.0}, {60.0, 50.0}, {180.0, 100.0}};
    }
    
    static std::vector<LoadProfile> moderate_ramp() {
        return {{0.0, 10.0}, {60.0, 100.0}, {180.0, 500.0}};
    }
    
    static std::vector<LoadProfile> stress_test() {
        return {{0.0, 10.0}, {30.0, 100.0}, {90.0, 500.0}, {150.0, 1000.0}};
    }
    
private:
    double fixed_rate_ = 10.0;
    std::vector<LoadProfile> profile_;
    bool use_profile_ = false;
};

} // namespace carbongrid
