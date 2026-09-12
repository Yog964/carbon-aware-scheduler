#include "greedy_scheduler.h"
#include "carbon_engine/carbon_engine.h"
#include <queue>

namespace carbongrid {

GreedyScheduler::GreedyScheduler() {}

GreedyScheduler::GreedyScheduler(const ScoringWeights& weights) : scoring_(weights) {}

std::string GreedyScheduler::schedule(const Task& task,
                                      std::vector<Node*>& feasible_nodes,
                                      const CarbonEngine& carbon_engine,
                                      double current_time) {
    if (feasible_nodes.empty()) {
        return "";
    }

    last_candidates_.clear();
    std::priority_queue<HeapEntry, std::vector<HeapEntry>, std::greater<HeapEntry>> min_heap;

    // Step 1: For each feasible node, compute score using ScoringFunction
    // Complexity: O(N) where N is number of feasible nodes
    for (Node* node : feasible_nodes) {
        // Fetch carbon data for the node's region
        double intensity = carbon_engine.get_carbon_intensity(node->region_id, current_time);
        double cost = carbon_engine.get_electricity_cost(node->region_id, current_time);
        double renewable_pct = carbon_engine.get_renewable_pct(node->region_id, current_time);

        NodeScore score = scoring_.compute(task, *node, intensity, cost, renewable_pct, current_time);
        
        // Step 2: Create HeapEntry for each
        HeapEntry entry;
        entry.score = score.total_score;
        entry.node_id = score.node_id;
        entry.detailed_score = score;
        
        // Step 3: Build a min-heap using std::priority_queue
        // Complexity: O(log N) per insertion, overall O(N log N)
        min_heap.push(entry);
        last_candidates_.push_back(entry); // Store for dashboard
    }

    if (min_heap.empty()) {
        return "";
    }

    // Step 4: Extract-min → this is the best node
    // Complexity: O(log N) to extract
    HeapEntry best = min_heap.top();

    // Step 5: Store the decision and all candidates for dashboard display
    last_decision_ = best.detailed_score;

    // Step 6: Return best node_id
    return best.node_id;
}

const NodeScore& GreedyScheduler::last_decision() const {
    return last_decision_;
}

const std::vector<HeapEntry>& GreedyScheduler::last_candidates() const {
    return last_candidates_;
}

}
