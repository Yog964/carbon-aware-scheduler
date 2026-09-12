#ifndef CARBONGUID_GREEDY_SCHEDULER_H
#define CARBONGUID_GREEDY_SCHEDULER_H

#include <string>
#include <vector>
#include <queue>
#include "scoring.h"
#include "models/task.h"
#include "models/node.h"
// Forward declaration for CarbonEngine
namespace carbongrid { class CarbonEngine; }

namespace carbongrid {

/*
 * GreedyScheduler implements the FAST PATH for urgent workloads.
 * 
 * DAA Concepts:
 * - Greedy Algorithm: Makes locally optimal choice at each step
 * - Min-Heap (Priority Queue): Maintains candidates ordered by score
 * - Time Complexity: O(N log N) where N = feasible nodes
 *   - O(N) to compute scores for all feasible nodes
 *   - O(N log N) to build the heap
 *   - O(log N) to extract minimum
 * - Space Complexity: O(N) for the heap
 * 
 * The greedy strategy works well for urgent tasks because:
 * 1. Speed: O(N log N) is fast enough for real-time scheduling
 * 2. Quality: The multi-factor scoring provides near-optimal results
 * 3. Simplicity: Single-task optimization avoids batch overhead
 */

struct HeapEntry {
    double score;
    std::string node_id;
    NodeScore detailed_score;
    
    // Min-heap: lower score = higher priority
    bool operator>(const HeapEntry& other) const {
        return score > other.score;
    }
};

class GreedyScheduler {
public:
    GreedyScheduler();
    explicit GreedyScheduler(const ScoringWeights& weights);
    
    // Schedule a single urgent task
    // Returns the best node_id, or empty string if no feasible node
    // Algorithm: Greedy selection via Min-Heap
    std::string schedule(const Task& task,
                         std::vector<Node*>& feasible_nodes,
                         const CarbonEngine& carbon_engine,
                         double current_time);
    
    // Get the last scheduling decision details
    const NodeScore& last_decision() const;
    
    // Get all candidate scores from last scheduling (for dashboard)
    const std::vector<HeapEntry>& last_candidates() const;
    
private:
    ScoringFunction scoring_;
    NodeScore last_decision_;
    std::vector<HeapEntry> last_candidates_;
};

}

#endif // CARBONGUID_GREEDY_SCHEDULER_H
