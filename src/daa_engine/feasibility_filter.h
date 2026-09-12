#ifndef CARBONGUID_FEASIBILITY_FILTER_H
#define CARBONGUID_FEASIBILITY_FILTER_H

#include <vector>
#include "models/task.h"
#include "models/node.h"

namespace carbongrid {

class FeasibilityFilter {
public:
    // Filter nodes that can accommodate the task
    // Returns vector of pointers to feasible nodes
    // Algorithm: Linear scan — O(N) where N = total nodes
    static std::vector<Node*> filter(const Task& task, std::vector<Node*>& all_nodes);
    
    // Check if a specific node can run the task within its deadline
    // Considers: CPU, RAM availability, and deadline feasibility
    static bool is_feasible(const Task& task, const Node& node, double current_time);
};

}

#endif // CARBONGUID_FEASIBILITY_FILTER_H
