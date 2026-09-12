#include "workload_generator.h"
#include "../utils/csv_parser.h"
#include <algorithm>
#include <iostream>

namespace carbongrid {

WorkloadGenerator::WorkloadGenerator() : WorkloadGenerator(WorkloadConfig{}) {}

WorkloadGenerator::WorkloadGenerator(const WorkloadConfig& config) 
    : config_(config), current_rate_(config.default_tasks_per_sec) {
    
    std::random_device rd;
    rng_ = std::mt19937(rd());
    
    cpu_dist_ = std::normal_distribution<double>(config_.cpu_mean, config_.cpu_stddev);
    ram_dist_ = std::normal_distribution<double>(config_.ram_mean, config_.ram_stddev);
    duration_dist_ = std::normal_distribution<double>(config_.duration_mean, config_.duration_stddev);
    arrival_dist_ = std::exponential_distribution<double>(current_rate_);
    priority_dist_ = std::discrete_distribution<int>(config_.priority_weights.begin(), config_.priority_weights.end());
    uniform_dist_ = std::uniform_real_distribution<double>(0.0, 1.0);
}

void WorkloadGenerator::load_config(const std::string& filepath) {
    // Basic stub for configuration loading
    // In a full implementation, parse JSON here
}

double WorkloadGenerator::clamp(double val, double min_val, double max_val) const {
    return std::max(min_val, std::min(val, max_val));
}

int WorkloadGenerator::generate_priority() {
    return priority_dist_(rng_) + 1; // 1 to 5
}

double WorkloadGenerator::generate_deadline(double arrival_time, double duration) {
    bool is_urgent = uniform_dist_(rng_) < config_.urgent_ratio;
    double multiplier;
    if (is_urgent) {
        // tighter bounds for urgent tasks
        multiplier = config_.deadline_slack_min_multiplier + uniform_dist_(rng_) * 0.5;
    } else {
        std::uniform_real_distribution<double> slack_dist(
            config_.deadline_slack_min_multiplier, config_.deadline_slack_max_multiplier);
        multiplier = slack_dist(rng_);
    }
    return arrival_time + duration * multiplier;
}

Task WorkloadGenerator::generate_task(double current_time) {
    Task t;
    t.task_id = "task_" + std::to_string(++task_counter_);
    t.arrival_time = current_time;
    
    t.cpu_required = clamp(cpu_dist_(rng_), config_.cpu_min, config_.cpu_max);
    t.ram_required = clamp(ram_dist_(rng_), config_.ram_min, config_.ram_max);
    t.execution_duration = clamp(duration_dist_(rng_), config_.duration_min, config_.duration_max);
    
    t.priority = generate_priority();
    t.deadline = generate_deadline(t.arrival_time, t.execution_duration);
    t.state = TaskState::QUEUED;
    
    return t;
}

std::vector<Task> WorkloadGenerator::generate_batch(double start_time, double end_time) {
    std::vector<Task> batch;
    double time = start_time;
    while (time < end_time) {
        batch.push_back(generate_task(time));
        time += next_inter_arrival_time();
    }
    return batch;
}

void WorkloadGenerator::load_trace(const std::string& filepath) {
    try {
        auto rows = CSVParser::parse(filepath, true);
        for (const auto& row : rows) {
            if (row.size() < 6) continue;
            Task t;
            t.task_id = row[0];
            t.arrival_time = std::stod(row[1]);
            t.cpu_required = std::stod(row[2]);
            t.ram_required = std::stod(row[3]);
            t.execution_duration = std::stod(row[4]);
            t.priority = std::stoi(row[5]);
            t.deadline = t.arrival_time + t.execution_duration * 2.0; // Simplistic
            t.state = TaskState::QUEUED;
            trace_tasks_.push_back(t);
        }
        std::sort(trace_tasks_.begin(), trace_tasks_.end(), [](const Task& a, const Task& b){
            return a.arrival_time < b.arrival_time;
        });
        trace_index_ = 0;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load trace: " << e.what() << "\n";
    }
}

Task WorkloadGenerator::next_trace_task(double current_time) {
    if (trace_index_ < trace_tasks_.size()) {
        Task t = trace_tasks_[trace_index_++];
        t.arrival_time = current_time; // Shift relative to current simulation time if necessary
        return t;
    }
    return generate_task(current_time); // Fallback
}

bool WorkloadGenerator::has_trace_tasks() const {
    return trace_index_ < trace_tasks_.size();
}

void WorkloadGenerator::set_arrival_rate(double tasks_per_sec) {
    current_rate_ = clamp(tasks_per_sec, config_.min_tasks_per_sec, config_.max_tasks_per_sec);
    arrival_dist_ = std::exponential_distribution<double>(current_rate_);
}

double WorkloadGenerator::get_arrival_rate() const {
    return current_rate_;
}

double WorkloadGenerator::next_inter_arrival_time() {
    return arrival_dist_(rng_);
}

int WorkloadGenerator::total_generated() const {
    return task_counter_;
}

} // namespace carbongrid
