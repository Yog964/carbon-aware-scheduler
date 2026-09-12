#include "metrics_engine.h"

namespace carbongrid {

MetricsEngine::MetricsEngine() = default;

void MetricsEngine::record_scheduling_event() {
    scheduling_events_++;
}

void MetricsEngine::record_task_scheduled(const Task& task) {
    scheduled_count_++;
}

void MetricsEngine::record_task_completed(const Task& task) {
    completed_count_++;
    total_latency_ += (task.completion_time - task.arrival_time);
    total_carbon_ += task.carbon_cost;
    total_cost_ += task.monetary_cost;
    if (task.completion_time > task.deadline) {
        record_deadline_violation(task);
    }
}

void MetricsEngine::record_task_failed(const Task& task) {
    failed_count_++;
}

void MetricsEngine::record_deadline_violation(const Task& task) {
    deadline_violations_++;
}

void MetricsEngine::record_scheduling_time(double time_ms) {
    total_scheduling_time_ms_ += time_ms;
}

void MetricsEngine::record_baseline_carbon(double carbon) {
    baseline_carbon_ += carbon;
}

void MetricsEngine::record_baseline_cost(double cost) {
    baseline_cost_ += cost;
}

void MetricsEngine::record_carbongrid_carbon(double carbon) {
    total_carbon_ += carbon;
}

void MetricsEngine::record_carbongrid_cost(double cost) {
    total_cost_ += cost;
}

MetricsSnapshot MetricsEngine::snapshot(double timestamp, const CloudState& cloud_state) const {
    MetricsSnapshot s;
    s.timestamp = timestamp;
    s.total_events = scheduling_events_;
    s.scheduled_count = scheduled_count_;
    s.pending_count = 0;
    s.queue_length = 0;
    s.avg_scheduling_time_ms = avg_scheduling_time_ms();
    s.throughput_tasks_per_sec = (timestamp > 0) ? static_cast<double>(completed_count_) / timestamp : 0.0;
    s.avg_latency = avg_latency();
    s.deadline_violations = deadline_violations_;
    s.deadline_miss_rate = deadline_miss_rate();
    s.task_completion_rate = task_completion_rate();
    s.total_carbon_emission = total_carbon_;
    s.total_cost = total_cost_;
    s.baseline_carbon = baseline_carbon_;
    s.baseline_cost = baseline_cost_;
    s.carbongrid_carbon = total_carbon_;
    s.carbongrid_cost = total_cost_;
    s.carbon_reduction_pct = carbon_reduction_pct();
    s.cost_reduction_pct = cost_reduction_pct();
    s.avg_cpu_utilization = cloud_state.avg_cpu_utilization();
    s.avg_ram_utilization = cloud_state.avg_ram_utilization();
    return s;
}

int MetricsEngine::total_scheduling_events() const { return scheduling_events_; }
int MetricsEngine::total_scheduled() const { return scheduled_count_; }
int MetricsEngine::total_completed() const { return completed_count_; }
int MetricsEngine::total_failed() const { return failed_count_; }
int MetricsEngine::total_deadline_violations() const { return deadline_violations_; }

double MetricsEngine::avg_scheduling_time_ms() const {
    if (scheduling_events_ == 0) return 0.0;
    return total_scheduling_time_ms_ / scheduling_events_;
}

double MetricsEngine::avg_latency() const {
    if (completed_count_ == 0) return 0.0;
    return total_latency_ / completed_count_;
}

double MetricsEngine::throughput(double elapsed_time) const {
    if (elapsed_time <= 0.0) return 0.0;
    return static_cast<double>(completed_count_) / elapsed_time;
}

double MetricsEngine::task_completion_rate() const {
    if (scheduled_count_ == 0) return 0.0;
    return static_cast<double>(completed_count_) / scheduled_count_;
}

double MetricsEngine::deadline_miss_rate() const {
    if (completed_count_ == 0) return 0.0;
    return static_cast<double>(deadline_violations_) / completed_count_;
}

double MetricsEngine::total_carbon() const { return total_carbon_; }
double MetricsEngine::total_cost() const { return total_cost_; }
double MetricsEngine::baseline_total_carbon() const { return baseline_carbon_; }
double MetricsEngine::baseline_total_cost() const { return baseline_cost_; }

double MetricsEngine::carbon_reduction_pct() const {
    if (baseline_carbon_ > 0.0) {
        return ((baseline_carbon_ - total_carbon_) / baseline_carbon_) * 100.0;
    }
    return 0.0;
}

double MetricsEngine::cost_reduction_pct() const {
    if (baseline_cost_ > 0.0) {
        return ((baseline_cost_ - total_cost_) / baseline_cost_) * 100.0;
    }
    return 0.0;
}

std::vector<MetricsSnapshot> MetricsEngine::get_history() const {
    return history_;
}

void MetricsEngine::take_snapshot(double timestamp, const CloudState& cloud_state) {
    history_.push_back(snapshot(timestamp, cloud_state));
}

void MetricsEngine::reset() {
    scheduling_events_ = 0;
    scheduled_count_ = 0;
    completed_count_ = 0;
    failed_count_ = 0;
    deadline_violations_ = 0;
    
    total_scheduling_time_ms_ = 0.0;
    total_latency_ = 0.0;
    total_carbon_ = 0.0;
    total_cost_ = 0.0;
    
    baseline_carbon_ = 0.0;
    baseline_cost_ = 0.0;
    
    history_.clear();
}

}
