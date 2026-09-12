#ifndef CARBONGUID_DAA_ENGINE_H
#define CARBONGUID_DAA_ENGINE_H

#include <vector>
#include <string>
#include "models/task.h"
#include "models/node.h"
#include "greedy_scheduler.h"
#include "mcmf_scheduler.h"
#include "feasibility_filter.h"

// Forward declaration for missing models
namespace carbongrid { 
    class CloudState; 
    class CarbonEngine; 
}

namespace carbongrid {

struct SchedulingDecision {
    std::string task_id;
    std::string node_id;
    std::string region_id;
    int time_slot;
    std::string algorithm_used; // "Greedy+Heap" or "MCMF+Dijkstra"
    double score;
    double scheduling_time_ms;
    bool success;
};

class DAAEngine {
public:
    DAAEngine(CloudState& cloud_state, CarbonEngine& carbon_engine);
    
    // Schedule a single task (auto-classifies urgent vs flexible)
    SchedulingDecision schedule(Task& task, double current_time);
    
    // Force fast path (for testing)
    SchedulingDecision schedule_urgent(Task& task, double current_time);
    
    // Schedule a batch of flexible tasks
    std::vector<SchedulingDecision> schedule_batch(std::vector<Task*>& tasks, double current_time);
    
    // Configuration
    void set_urgency_threshold(double threshold_seconds);
    void set_batch_window_size(int max_tasks);
    void set_scoring_weights(const ScoringWeights& weights);
    
    // Get recent decisions for dashboard
    const std::vector<SchedulingDecision>& recent_decisions() const;
    
private:
    CloudState& cloud_state_;
    CarbonEngine& carbon_engine_;
    
    GreedyScheduler greedy_;
    MCMFScheduler mcmf_;
    FeasibilityFilter filter_;
    
    double urgency_threshold_ = 30.0; // seconds
    int batch_window_size_ = 20;
    
    std::vector<SchedulingDecision> recent_decisions_;
    
    // Batch accumulator for flexible tasks
    std::vector<Task*> flexible_batch_;
};

}

#endif // CARBONGUID_DAA_ENGINE_H
