#pragma once

#include <string>
#include <vector>
#include <random>
#include <functional>
#include "../models/task.h"

namespace carbongrid {

struct WorkloadConfig {
    double default_tasks_per_sec = 10.0;
    double min_tasks_per_sec = 1.0;
    double max_tasks_per_sec = 1000.0;
    
    double cpu_min = 0.5, cpu_max = 8.0, cpu_mean = 2.0, cpu_stddev = 1.5;
    double ram_min = 0.5, ram_max = 32.0, ram_mean = 4.0, ram_stddev = 3.0;
    double duration_min = 5.0, duration_max = 600.0, duration_mean = 60.0, duration_stddev = 45.0;
    
    std::vector<double> priority_weights = {0.1, 0.2, 0.35, 0.25, 0.1};
    double deadline_slack_min_multiplier = 1.2;
    double deadline_slack_max_multiplier = 5.0;
    double urgent_ratio = 0.3;
};

class WorkloadGenerator {
public:
    WorkloadGenerator();
    explicit WorkloadGenerator(const WorkloadConfig& config);
    
    // Load config from JSON file
    void load_config(const std::string& filepath);
    
    // Generate a single task at the given simulation time
    Task generate_task(double current_time);
    
    // Generate a batch of tasks for a time window
    std::vector<Task> generate_batch(double start_time, double end_time);
    
    // Load and replay tasks from Alibaba trace CSV
    void load_trace(const std::string& filepath);
    Task next_trace_task(double current_time);
    bool has_trace_tasks() const;
    
    // Rate control
    void set_arrival_rate(double tasks_per_sec);
    double get_arrival_rate() const;
    
    // Get time until next task arrival (exponential distribution)
    double next_inter_arrival_time();
    
    // Stats
    int total_generated() const;
    
private:
    WorkloadConfig config_;
    double current_rate_;
    int task_counter_ = 0;
    
    std::mt19937 rng_;
    std::normal_distribution<double> cpu_dist_;
    std::normal_distribution<double> ram_dist_;
    std::normal_distribution<double> duration_dist_;
    std::exponential_distribution<double> arrival_dist_;
    std::discrete_distribution<int> priority_dist_;
    std::uniform_real_distribution<double> uniform_dist_;
    
    // Trace replay
    std::vector<Task> trace_tasks_;
    size_t trace_index_ = 0;
    
    double clamp(double val, double min_val, double max_val) const;
    int generate_priority();
    double generate_deadline(double arrival_time, double duration);
};

} // namespace carbongrid
