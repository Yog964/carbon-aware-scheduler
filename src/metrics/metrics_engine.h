#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include "../models/metrics.h"
#include "../models/task.h"
#include "../cloud_state/cloud_state.h"

namespace carbongrid {

class MetricsEngine {
public:
    MetricsEngine();
    
    // Record events
    void record_scheduling_event();
    void record_task_scheduled(const Task& task);
    void record_task_completed(const Task& task);
    void record_task_failed(const Task& task);
    void record_deadline_violation(const Task& task);
    void record_scheduling_time(double time_ms);
    
    // Baseline tracking
    void record_baseline_carbon(double carbon);
    void record_baseline_cost(double cost);
    void record_carbongrid_carbon(double carbon);
    void record_carbongrid_cost(double cost);
    
    // Snapshot current metrics
    MetricsSnapshot snapshot(double timestamp, const CloudState& cloud_state) const;
    
    // Individual metric queries
    int total_scheduling_events() const;
    int total_scheduled() const;
    int total_completed() const;
    int total_failed() const;
    int total_deadline_violations() const;
    
    double avg_scheduling_time_ms() const;
    double avg_latency() const;
    double throughput(double elapsed_time) const;
    double task_completion_rate() const;
    double deadline_miss_rate() const;
    
    // Carbon/cost totals
    double total_carbon() const;
    double total_cost() const;
    double baseline_total_carbon() const;
    double baseline_total_cost() const;
    double carbon_reduction_pct() const;
    double cost_reduction_pct() const;
    
    // History for charts
    std::vector<MetricsSnapshot> get_history() const;
    void take_snapshot(double timestamp, const CloudState& cloud_state);
    
    // Reset
    void reset();
    
private:
    int scheduling_events_ = 0;
    int scheduled_count_ = 0;
    int completed_count_ = 0;
    int failed_count_ = 0;
    int deadline_violations_ = 0;
    
    double total_scheduling_time_ms_ = 0.0;
    double total_latency_ = 0.0;
    double total_carbon_ = 0.0;
    double total_cost_ = 0.0;
    
    double baseline_carbon_ = 0.0;
    double baseline_cost_ = 0.0;
    
    std::vector<MetricsSnapshot> history_;
};

}
