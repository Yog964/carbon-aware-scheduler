#ifndef CARBONGUID_SCORING_H
#define CARBONGUID_SCORING_H

#include <string>
#include <vector>
#include "models/task.h"
#include "models/node.h"

namespace carbongrid {

struct ScoringWeights {
    double carbon_weight = 0.40;   // w1: carbon impact
    double cost_weight = 0.20;     // w2: monetary cost
    double load_weight = 0.25;     // w3: resource utilization balance
    double deadline_weight = 0.15; // w4: deadline urgency penalty
};

struct NodeScore {
    std::string node_id;
    double total_score;      // Combined score (lower = better)
    double carbon_score;     // Normalized carbon component
    double cost_score;       // Normalized cost component
    double load_score;       // Normalized load component
    double deadline_score;   // Normalized deadline penalty
};

class ScoringFunction {
public:
    ScoringFunction();
    explicit ScoringFunction(const ScoringWeights& weights);
    
    // Compute score for placing task on node
    // Score formula: w1*carbon + w2*cost + w3*load + w4*deadline_penalty
    // Lower score = better placement
    NodeScore compute(const Task& task, const Node& node,
                      double carbon_intensity, double electricity_cost,
                      double renewable_pct, double current_time) const;
    
    // Compute scores for all feasible nodes
    std::vector<NodeScore> compute_all(const Task& task,
                                        const std::vector<Node*>& nodes,
                                        double current_time) const;
    
    void set_weights(const ScoringWeights& weights);
    const ScoringWeights& get_weights() const;
    
private:
    ScoringWeights weights_;
    
    // Normalize carbon intensity to [0,1]: higher intensity → higher score (worse)
    double normalize_carbon(double carbon_intensity, double renewable_pct) const;
    
    // Normalize cost to [0,1]: higher cost → higher score
    double normalize_cost(double cost) const;
    
    // Normalize load: higher utilization → higher score (prefer less loaded nodes)
    double normalize_load(const Node& node) const;
    
    // Deadline penalty: tighter slack → higher penalty
    double compute_deadline_penalty(const Task& task, double current_time) const;
};

}

#endif // CARBONGUID_SCORING_H
