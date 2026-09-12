#ifndef CARBONGUID_GRAPH_H
#define CARBONGUID_GRAPH_H

#include <vector>

namespace carbongrid {

/*
 * Flow network graph for Min-Cost Max-Flow algorithm.
 * 
 * The graph models the scheduling problem as a network flow:
 * - Source (S) → Task nodes → Node-TimeSlot nodes → Sink (T)
 * - Edge capacities represent resource constraints
 * - Edge costs represent carbon + monetary + penalty costs
 * 
 * DAA Concepts:
 * - Graph Representation: Adjacency list with forward/backward edges
 * - Network Flow: Capacity and flow on each edge
 * - Residual Graph: Maintained implicitly via reverse edges
 */

struct FlowEdge {
    int to;           // Target node index
    int rev;          // Index of reverse edge in adj[to]
    double capacity;  // Edge capacity
    double cost;      // Cost per unit of flow
    double flow;      // Current flow
    
    double residual_capacity() const { return capacity - flow; }
};

class FlowGraph {
public:
    explicit FlowGraph(int num_nodes) : n_(num_nodes), graph_(num_nodes) {}
    
    // Add an edge with capacity and cost (also adds reverse edge)
    void add_edge(int from, int to, double capacity, double cost) {
        graph_[from].push_back({to, static_cast<int>(graph_[to].size()), capacity, cost, 0.0});
        graph_[to].push_back({from, static_cast<int>(graph_[from].size() - 1), 0.0, -cost, 0.0});
    }
    
    // Get adjacency list
    std::vector<FlowEdge>& adj(int node) { return graph_[node]; }
    const std::vector<FlowEdge>& adj(int node) const { return graph_[node]; }
    
    int num_nodes() const { return n_; }
    
    // Reset all flows
    void reset_flow() {
        for (auto& adj_list : graph_) {
            for (auto& edge : adj_list) {
                edge.flow = 0.0;
            }
        }
    }
    
private:
    int n_;
    std::vector<std::vector<FlowEdge>> graph_;
};

}

#endif // CARBONGUID_GRAPH_H
