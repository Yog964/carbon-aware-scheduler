#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>

namespace carbongrid {

/**
 * @brief Represents a compute node in the CarbonGrid system.
 * Contains capacity and utilization metrics for DAA scheduling decisions.
 */
struct Node {
    std::string node_id;
    std::string region_id;
    
    // Capacities
    double cpu_capacity;
    double ram_capacity;
    
    // Utilization
    double cpu_used = 0.0;
    double ram_used = 0.0;
    
    // Status
    bool is_available = true;
    std::vector<std::string> running_tasks;
    
    // Environmental
    double power_usage_effectiveness; // PUE

    double cpu_available() const {
        return cpu_capacity - cpu_used;
    }

    double ram_available() const {
        return ram_capacity - ram_used;
    }

    double cpu_utilization() const {
        if (cpu_capacity == 0.0) return 0.0;
        return (cpu_used / cpu_capacity) * 100.0;
    }

    double ram_utilization() const {
        if (ram_capacity == 0.0) return 0.0;
        return (ram_used / ram_capacity) * 100.0;
    }

    bool can_fit(double cpu, double ram) const {
        return is_available && cpu_available() >= cpu && ram_available() >= ram;
    }

    void allocate(double cpu, double ram, const std::string& task_id) {
        if (!can_fit(cpu, ram)) {
            throw std::runtime_error("Node capacity exceeded");
        }
        cpu_used += cpu;
        ram_used += ram;
        running_tasks.push_back(task_id);
    }

    void release(double cpu, double ram, const std::string& task_id) {
        cpu_used = std::max(0.0, cpu_used - cpu);
        ram_used = std::max(0.0, ram_used - ram);
        auto it = std::find(running_tasks.begin(), running_tasks.end(), task_id);
        if (it != running_tasks.end()) {
            running_tasks.erase(it);
        }
    }
};

} // namespace carbongrid
