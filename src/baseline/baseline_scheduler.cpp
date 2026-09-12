#include "baseline_scheduler.h"
#include <limits>
#include <cstdlib>

namespace carbongrid {

BaselineScheduler::BaselineScheduler(BaselineStrategy strategy)
    : strategy_(strategy), round_robin_index_(0) {}

std::string BaselineScheduler::select_node(const Task& task, CloudState& cloud_state) {
    switch (strategy_) {
        case BaselineStrategy::ROUND_ROBIN:
            return round_robin_select(task, cloud_state);
        case BaselineStrategy::LEAST_LOADED:
            return least_loaded_select(task, cloud_state);
        case BaselineStrategy::RANDOM:
            return random_select(task, cloud_state);
        default:
            return "";
    }
}

void BaselineScheduler::set_strategy(BaselineStrategy strategy) {
    strategy_ = strategy;
}

BaselineStrategy BaselineScheduler::get_strategy() const {
    return strategy_;
}

void BaselineScheduler::reset() {
    round_robin_index_ = 0;
}

std::string BaselineScheduler::round_robin_select(const Task& task, CloudState& cloud_state) {
    auto nodes = cloud_state.get_all_nodes();
    if (nodes.empty()) return "";
    
    int num_nodes = static_cast<int>(nodes.size());
    for (int i = 0; i < num_nodes; ++i) {
        int idx = (round_robin_index_ + i) % num_nodes;
        auto* node = nodes[idx];
        
        if (!node->is_available) continue;
        // DAA complexity: O(N) linear scan with round-robin start position
        if (node->can_fit(task.cpu_required, task.ram_required)) {
            round_robin_index_ = (idx + 1) % num_nodes;
            return node->node_id;
        }
    }
    
    return "";
}

std::string BaselineScheduler::least_loaded_select(const Task& task, CloudState& cloud_state) {
    auto nodes = cloud_state.get_all_nodes();
    std::string best_node_id = "";
    double min_util = std::numeric_limits<double>::max();
    
    // DAA complexity: O(N) linear scan to find minimum utilization node
    for (auto* node : nodes) {
        if (!node->is_available) continue;
        if (node->can_fit(task.cpu_required, task.ram_required)) {
            double util = node->cpu_utilization();
            if (util < min_util) {
                min_util = util;
                best_node_id = node->node_id;
            }
        }
    }
    
    return best_node_id;
}

std::string BaselineScheduler::random_select(const Task& task, CloudState& cloud_state) {
    auto nodes = cloud_state.get_all_nodes();
    std::vector<std::string> feasible_nodes;
    
    for (auto* node : nodes) {
        if (!node->is_available) continue;
        if (node->can_fit(task.cpu_required, task.ram_required)) {
            feasible_nodes.push_back(node->node_id);
        }
    }
    
    if (feasible_nodes.empty()) return "";
    
    int idx = std::rand() % static_cast<int>(feasible_nodes.size());
    return feasible_nodes[idx];
}

}
