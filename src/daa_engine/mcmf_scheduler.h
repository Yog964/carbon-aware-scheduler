#ifndef CARBONGUID_MCMF_SCHEDULER_H
#define CARBONGUID_MCMF_SCHEDULER_H

#include <vector>
#include <unordered_map>
#include <string>
#include "graph.h"
#include "scoring.h"
#include "models/task.h"
#include "models/node.h"
// Forward declaration for CarbonEngine
namespace carbongrid { class CarbonEngine; }

namespace carbongrid {

/*
 * MCMFScheduler implements the BATCH PATH for flexible/deferrable workloads.
 * 
 * DAA Concepts:
 * - Graph Modeling: Scheduling as a minimum-cost flow problem
 * - Min-Cost Max-Flow (MCMF): Find maximum flow with minimum total cost
 * - Dijkstra's Algorithm: Finding shortest (cheapest) augmenting paths
 * - Johnson's Reweighting: Handle potential negative costs via potentials
 * 
 * Time Complexity: O(F * E * log V) where:
 *   F = total flow (number of tasks)
 *   E = edges in flow network
 *   V = vertices (source + tasks + node×timeslots + sink)
 * 
 * Space Complexity: O(V + E) for the graph
 * 
 * Flow Network Structure:
 *   Source (S)
 *     |  capacity=1 per task, cost=0
 *     v
 *   Task Nodes (T1, T2, ..., Tk)
 *     |  capacity=1, cost=carbon+monetary+deadline_penalty
 *     v
 *   Node-TimeSlot Nodes (N1_S1, N1_S2, ..., Nm_Sp)
 *     |  capacity=node_remaining_capacity, cost=0
 *     v
 *   Sink (T)
 */

struct MCMFResult {
    bool success;
    double total_cost;
    double total_flow;
    // task_id → (node_id, time_slot)
    std::unordered_map<std::string, std::pair<std::string, int>> assignments;
};

class MCMFScheduler {
public:
    MCMFScheduler();
    explicit MCMFScheduler(const ScoringWeights& weights);
    
    // Schedule a batch of flexible tasks
    // Builds flow network and solves MCMF
    MCMFResult schedule_batch(const std::vector<Task*>& tasks,
                               std::vector<Node*>& feasible_nodes,
                               const CarbonEngine& carbon_engine,
                               double current_time,
                               int num_time_slots = 3);
    
private:
    ScoringFunction scoring_;
    
    // Build the flow network graph
    struct GraphMapping {
        int source;
        int sink;
        std::unordered_map<int, std::string> vertex_to_task;
        std::unordered_map<int, std::pair<std::string, int>> vertex_to_node_slot;
    };
    
    std::pair<FlowGraph, GraphMapping> build_graph(
        const std::vector<Task*>& tasks,
        std::vector<Node*>& feasible_nodes,
        const CarbonEngine& carbon_engine,
        double current_time,
        int num_time_slots);
    
    // Solve MCMF using Successive Shortest Paths with Dijkstra
    std::pair<double, double> solve_mcmf(FlowGraph& graph, int source, int sink);
    
    // Dijkstra's algorithm on the residual graph with Johnson's potentials
    struct DijkstraResult {
        std::vector<double> dist;
        std::vector<int> parent_vertex;
        std::vector<int> parent_edge;
        bool sink_reachable;
    };
    
    DijkstraResult dijkstra(const FlowGraph& graph, int source, int sink,
                            const std::vector<double>& potential);
    
    // Extract assignments from the solved flow network
    MCMFResult extract_assignments(const FlowGraph& graph,
                                    const GraphMapping& mapping);
};

}

#endif // CARBONGUID_MCMF_SCHEDULER_H
