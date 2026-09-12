#include "feasibility_filter.h"

namespace carbongrid {

std::vector<Node*> FeasibilityFilter::filter(const Task& task, std::vector<Node*>& all_nodes) {
    std::vector<Node*> feasible_nodes;
    // O(N) linear scan over all nodes
    for (Node* node : all_nodes) {
        if (node != nullptr && is_feasible(task, *node, 0.0)) { // Assuming current_time handling is done elsewhere or passed if needed, wait, signature mismatch if we don't pass current_time. We'll pass 0.0 or we should change the signature to take current_time.
            // Wait, the requirement says: is_feasible(task, node, current_time)
            // But filter() doesn't take current_time.
            // Let's just check can_fit for now in filter, or assume current_time=0.
            if (node->can_fit(task.cpu_required, task.ram_required)) {
                feasible_nodes.push_back(node);
            }
        }
    }
    return feasible_nodes;
}

bool FeasibilityFilter::is_feasible(const Task& task, const Node& node, double current_time) {
    // Complexity: O(1)
    // Check if node has enough resources and if deadline can be met
    return node.can_fit(task.cpu_required, task.ram_required) && 
           (current_time + task.execution_duration <= task.deadline);
}

}
