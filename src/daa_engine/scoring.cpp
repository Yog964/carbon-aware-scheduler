#include "scoring.h"
#include <algorithm>

namespace {
    // GCC 6.3 does not support std::clamp; use manual implementation
    double clamp_val(double v, double lo, double hi) {
        return std::max(lo, std::min(v, hi));
    }
}

namespace carbongrid {

ScoringFunction::ScoringFunction() {}

ScoringFunction::ScoringFunction(const ScoringWeights& weights) : weights_(weights) {}

NodeScore ScoringFunction::compute(const Task& task, const Node& node,
                                   double carbon_intensity, double electricity_cost,
                                   double renewable_pct, double current_time) const {
    NodeScore score;
    score.node_id = node.node_id;
    
    score.carbon_score = normalize_carbon(carbon_intensity, renewable_pct);
    score.cost_score = normalize_cost(electricity_cost);
    score.load_score = normalize_load(node);
    score.deadline_score = compute_deadline_penalty(task, current_time);
    
    score.total_score = weights_.carbon_weight * score.carbon_score +
                        weights_.cost_weight * score.cost_score +
                        weights_.load_weight * score.load_score +
                        weights_.deadline_weight * score.deadline_score;
                        
    return score;
}

std::vector<NodeScore> ScoringFunction::compute_all(const Task& task,
                                                    const std::vector<Node*>& nodes,
                                                    double current_time) const {
    std::vector<NodeScore> scores;
    scores.reserve(nodes.size());
    // This is an O(N) operation
    for (const Node* node : nodes) {
        // Mock values for carbon data if not passed directly, but the signature doesn't take it.
        // The user specifies "/* carbon data per node needed */". 
        // We will just use placeholder values here since compute_all lacks carbon engine access.
        // It's better if the caller uses compute() with carbon_engine data directly.
        scores.push_back(compute(task, *node, 400.0, 0.10, 20.0, current_time));
    }
    return scores;
}

void ScoringFunction::set_weights(const ScoringWeights& weights) {
    weights_ = weights;
}

const ScoringWeights& ScoringFunction::get_weights() const {
    return weights_;
}

double ScoringFunction::normalize_carbon(double carbon_intensity, double renewable_pct) const {
    // carbon_intensity ranges roughly 50-800 gCO2/kWh
    double norm_intensity = (carbon_intensity - 50.0) / 750.0;
    double score = 0.7 * norm_intensity + 0.3 * (1.0 - renewable_pct / 100.0);
    return clamp_val(score, 0.0, 1.0);
}

double ScoringFunction::normalize_cost(double cost) const {
    // typical range $0.03-0.20/kWh
    double norm_cost = (cost - 0.03) / 0.17;
    return clamp_val(norm_cost, 0.0, 1.0);
}

double ScoringFunction::normalize_load(const Node& node) const {
    // cpu_utilization + ram_utilization / 200.0
    double cpu_util = (node.cpu_capacity > 0) ? (node.cpu_used * 100.0 / node.cpu_capacity) : 100.0;
    double ram_util = (node.ram_capacity > 0) ? (node.ram_used * 100.0 / node.ram_capacity) : 100.0;
    double norm_load = (cpu_util + ram_util) / 200.0;
    return clamp_val(norm_load, 0.0, 1.0);
}

double ScoringFunction::compute_deadline_penalty(const Task& task, double current_time) const {
    double slack = task.deadline - current_time - task.execution_duration;
    if (slack <= 0) return 1.0;
    if (slack > 300.0) return 0.0;
    return 1.0 - (slack / 300.0);
}

}
