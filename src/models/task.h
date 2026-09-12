#pragma once

#include <string>
#include "node.h"

namespace carbongrid {

enum class TaskState {
    QUEUED,
    SCHEDULED,
    RUNNING,
    COMPLETED,
    FAILED
};

/**
 * @brief Represents a task or pod in the CarbonGrid system.
 * Provides properties and helper methods for scheduling and simulation.
 */
struct Task {
    std::string task_id;
    double arrival_time; // seconds
    double cpu_required; // cores
    double ram_required; // GB
    double execution_duration; // seconds
    int priority; // 1=highest, 5=lowest
    double deadline; // absolute time in seconds
    
    TaskState state = TaskState::QUEUED;
    
    std::string assigned_node_id = "";
    std::string assigned_region = "";
    int assigned_time_slot = -1;
    double scheduled_time = 0.0;
    double completion_time = 0.0;
    
    double carbon_cost = 0.0; // Carbon emitted for this task
    double monetary_cost = 0.0;

    /**
     * @brief Calculates the remaining slack time. O(1) time complexity.
     */
    double slack(double current_time) const {
        return deadline - (current_time + execution_duration);
    }

    /**
     * @brief Checks if task is urgently nearing its deadline. O(1) time complexity.
     */
    bool is_urgent(double current_time, double threshold = 30.0) const {
        return slack(current_time) < threshold;
    }

    /**
     * @brief Determines if the given node has sufficient capacity for this task. O(1) time complexity.
     */
    bool is_feasible_on_node(const Node& node) const {
        return node.can_fit(cpu_required, ram_required);
    }
};

} // namespace carbongrid
