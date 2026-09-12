#include "daa_engine.h"
#include "cloud_state/cloud_state.h"
#include "carbon_engine/carbon_engine.h"
#include <chrono>

namespace carbongrid {

DAAEngine::DAAEngine(CloudState& cloud_state, CarbonEngine& carbon_engine)
    : cloud_state_(cloud_state), carbon_engine_(carbon_engine) {
}

SchedulingDecision DAAEngine::schedule(Task& task, double current_time) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // 1. Get all nodes from cloud_state
    std::vector<Node*> all_nodes = cloud_state_.get_all_nodes();
    
    // 2. Run FeasibilityFilter::filter
    std::vector<Node*> feasible_nodes = filter_.filter(task, all_nodes);
    
    // 3. If no feasible nodes, return failure decision
    if (feasible_nodes.empty()) {
        return {task.task_id, "", "", -1, "None", 0.0, 0.0, false};
    }
    
    // 4. Check task.is_urgent(current_time, urgency_threshold_)
    // Use slack method to determine urgency if is_urgent isn't directly available with threshold
    double slack = task.deadline - current_time - task.execution_duration;
    bool is_urgent = (slack <= urgency_threshold_);
    
    // 5. If URGENT: call greedy_.schedule()
    if (is_urgent) {
        std::string node_id = greedy_.schedule(task, feasible_nodes, carbon_engine_, current_time);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        double time_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
        
        SchedulingDecision decision;
        decision.task_id = task.task_id;
        decision.node_id = node_id;
        decision.success = !node_id.empty();
        if (decision.success) {
            decision.region_id = cloud_state_.get_node(node_id).region_id;
            decision.score = greedy_.last_decision().total_score;
        }
        decision.time_slot = 0;
        decision.algorithm_used = "Greedy+Heap";
        decision.scheduling_time_ms = time_ms;
        
        // 8. Store decision in recent_decisions_
        recent_decisions_.push_back(decision);
        if (recent_decisions_.size() > 100) {
            recent_decisions_.erase(recent_decisions_.begin());
        }
        
        return decision;
    } 
    // 6. If FLEXIBLE: add to flexible_batch_
    else {
        flexible_batch_.push_back(&task);
        if (flexible_batch_.size() >= static_cast<size_t>(batch_window_size_)) {
            // Trigger batch schedule
            schedule_batch(flexible_batch_, current_time);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        double time_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
        
        return {task.task_id, "", "", -1, "Deferred-MCMF", 0.0, time_ms, false};
    }
}

SchedulingDecision DAAEngine::schedule_urgent(Task& task, double current_time) {
    auto start_time = std::chrono::high_resolution_clock::now();
    std::vector<Node*> all_nodes = cloud_state_.get_all_nodes();
    std::vector<Node*> feasible_nodes = filter_.filter(task, all_nodes);
    
    std::string node_id = greedy_.schedule(task, feasible_nodes, carbon_engine_, current_time);
    auto end_time = std::chrono::high_resolution_clock::now();
    double time_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    
    SchedulingDecision decision;
    decision.task_id = task.task_id;
    decision.node_id = node_id;
    if (!node_id.empty()) {
        decision.region_id = cloud_state_.get_node(node_id).region_id;
    }
    decision.time_slot = 0;
    decision.algorithm_used = "Greedy+Heap";
    decision.score = greedy_.last_decision().total_score;
    decision.scheduling_time_ms = time_ms;
    decision.success = !node_id.empty();
    
    recent_decisions_.push_back(decision);
    if (recent_decisions_.size() > 100) {
        recent_decisions_.erase(recent_decisions_.begin());
    }
    
    return decision;
}

std::vector<SchedulingDecision> DAAEngine::schedule_batch(std::vector<Task*>& tasks, double current_time) {
    auto start_time = std::chrono::high_resolution_clock::now();
    std::vector<SchedulingDecision> decisions;
    
    // 1. Get all nodes, filter feasible
    std::vector<Node*> all_nodes = cloud_state_.get_all_nodes();
    // For batching we might just pass all nodes and let MCMF graph capacities handle it,
    // or filter nodes that are basically capable
    std::vector<Node*> feasible_nodes;
    for (Node* n : all_nodes) {
        if (n->cpu_capacity > 0) feasible_nodes.push_back(n);
    }
    
    // 2. Call mcmf_.schedule_batch()
    MCMFResult result = mcmf_.schedule_batch(tasks, feasible_nodes, carbon_engine_, current_time, 3);
    
    auto end_time = std::chrono::high_resolution_clock::now();
    double time_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    
    // 3. Build SchedulingDecision for each assignment
    for (Task* t : tasks) {
        SchedulingDecision dec;
        dec.task_id = t->task_id;
        dec.algorithm_used = "MCMF+Dijkstra";
        dec.scheduling_time_ms = time_ms / tasks.size(); // distribute time
        
        if (result.assignments.count(t->task_id)) {
            auto node_slot = result.assignments[t->task_id];
            dec.node_id = node_slot.first;
            dec.time_slot = node_slot.second;
            dec.region_id = cloud_state_.get_node(dec.node_id).region_id;
            dec.success = true;
        } else {
            dec.success = false;
        }
        decisions.push_back(dec);
        
        recent_decisions_.push_back(dec);
        if (recent_decisions_.size() > 100) {
            recent_decisions_.erase(recent_decisions_.begin());
        }
    }
    
    // 4. Clear flexible_batch_
    flexible_batch_.clear();
    
    return decisions;
}

void DAAEngine::set_urgency_threshold(double threshold_seconds) {
    urgency_threshold_ = threshold_seconds;
}

void DAAEngine::set_batch_window_size(int max_tasks) {
    batch_window_size_ = max_tasks;
}

void DAAEngine::set_scoring_weights(const ScoringWeights& weights) {
    greedy_ = GreedyScheduler(weights);
    mcmf_ = MCMFScheduler(weights);
}

const std::vector<SchedulingDecision>& DAAEngine::recent_decisions() const {
    return recent_decisions_;
}

}
