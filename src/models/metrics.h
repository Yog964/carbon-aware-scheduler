#pragma once

#include <string>
#include <map>
#include <utility>

namespace carbongrid {

/**
 * @brief Captures a point-in-time snapshot of system metrics.
 */
struct MetricsSnapshot {
    double timestamp = 0.0;
    
    // Scheduling
    int total_events = 0;
    int scheduled_count = 0;
    int pending_count = 0;
    int queue_length = 0;
    double avg_scheduling_time_ms = 0.0;
    double throughput_tasks_per_sec = 0.0;
    
    // Resource
    double avg_cpu_utilization = 0.0;
    double avg_ram_utilization = 0.0;
    std::map<std::string, std::pair<double, double>> per_node_utilization; // map of node_id -> {cpu_util, ram_util}
    
    // Environmental
    double total_carbon_emission = 0.0;
    double avg_carbon_intensity = 0.0;
    double carbon_reduction_pct = 0.0;
    
    // Economic
    double total_cost = 0.0;
    double cost_reduction_pct = 0.0;
    
    // Performance
    double avg_latency = 0.0;
    int deadline_violations = 0;
    double deadline_miss_rate = 0.0;
    double task_completion_rate = 0.0;
    
    // Comparison
    double baseline_carbon = 0.0;
    double baseline_cost = 0.0;
    double carbongrid_carbon = 0.0;
    double carbongrid_cost = 0.0;
};

} // namespace carbongrid
